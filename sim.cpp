#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <algorithm> //random_shuffle
#include <ctime>

using namespace std;
using namespace cv;

static int node_size = 10;
static int width = 800;
static int height = 800;

class NODE {

public:
	string color;
	bool reproduced = false;
	string bey2[2];
	string gey[2];
};


void preload_init_node_map(vector<vector<NODE> > &node_map, string path);
void random_init_node_map(vector<vector<NODE> > &node_map);
void determine_color(vector<vector<NODE> > &node_map);
void draw_node_map(vector<vector<NODE> > &node_map, Mat &mat);
void draw_raster(Mat &mat);
vector<vector<NODE> > evolutionize_node_map(vector<vector<NODE> > &node_map);

// node_size, width, height, step / auto, auto-steps
int main(int argc, char** argv) {
	srand (time(NULL));
	
	
	if (argc != 7) {
		cout << "sim usage:\n./sim node_size, width, height, step_graphic on{1}/off{0}, step_nr, node_map_file" << endl;
		cout << "** NOTE ** you should rather use sim_loader.bat" << endl;
		return 1;
	}
	
	node_size = atoi(argv[1]);
	width = atoi(argv[2]);
	height = atoi(argv[3]);
	
	bool stepwise = atoi(argv[4]) == -1 ? false : true;
	int step_nr = atoi(argv[5]);
	string filename(argv[6]);
	
	
	if (filename != "null") {
		Mat mat = imread(filename, CV_LOAD_IMAGE_COLOR);
		width = mat.size().width * node_size;
		height = mat.size().height * node_size;
	}
	
	Mat mat(height, width, CV_8UC3, Scalar(0,0,0));
	vector<vector<NODE> > node_map(height/node_size, vector<NODE>(width/node_size));
	
	if (filename == "null") {
		random_init_node_map(node_map);
	} else {
		preload_init_node_map(node_map, filename);
	}
	
	
	if (stepwise == true) {
	
		while (1) {
			
			determine_color(node_map);
			draw_node_map(node_map, mat);
			draw_raster(mat);
			imshow("Simulation Augenfarbe", mat);
			waitKey(atoi(argv[4]));
			node_map = evolutionize_node_map(node_map);
		}
		
	} else {
		
		for (int i = 0; i < step_nr; i++) {
			clock_t time_a = clock();
			node_map = evolutionize_node_map(node_map);
			clock_t time_b = clock();
			unsigned int total_time_ticks = (unsigned int)(time_b - time_a);
			cout << "evolution step " << i + 1 << "/" << step_nr << " " <<total_time_ticks << "ms" <<endl;
		}
		
		determine_color(node_map);
		draw_node_map(node_map, mat);
		draw_raster(mat);
		imshow("Simulation Augenfarbe", mat);
		waitKey(0);
	}
	
	
}

vector<vector<NODE> > evolutionize_node_map(vector<vector<NODE> > &node_map) {
	
	vector<vector<NODE> > new_node_map(height/node_size, vector<NODE>(width/node_size));
	
	vector<int> width_values(width / node_size);
	vector<int> height_values(height / node_size);
	
	for(int x = 0; x < width / node_size; x++) {
		width_values.at(x) = x;
	}
	
	for(int x = 0; x < height / node_size; x++) {
		height_values.at(x) = x;
	}
	
	random_shuffle(width_values.begin(), width_values.end());
	random_shuffle(height_values.begin(), height_values.end());
	
	for (int x = 0; x < width_values.size(); x++) {
		for (int y = 0; y < height_values.size(); y++) {
			
			if (node_map.at(y).at(x).reproduced == true) continue;
			
			int x_edge = (x - 2) < 0 ? 0 : x - 2;
			int y_edge = (y - 2) < 0 ? 0 : y - 2;
			
			vector<vector<int> > partner_nodes;
			
			// gehe 5x5 matrix durch und pushe nicht reproduzierte in vector, dann randomize vector und wähle partner.at(0)
			for (int i = x_edge; i < x_edge + 5; i++) {
				for (int n = y_edge; n < y_edge + 5; n++) {
					
					if (i > (width_values.size() - 1) || n > (height_values.size() - 1)) continue;
					if (node_map.at(n).at(i).reproduced == true) continue;
					if (i == x && n == y) continue;
					
					vector<int> pos(2);
					pos.at(0) = i;
					pos.at(1) = n;
					partner_nodes.push_back(pos);
				}
			}
			random_shuffle(partner_nodes.begin(), partner_nodes.end());
			
			int x_partner;
			int y_partner;
			
			if (partner_nodes.empty()) {
				// wähle zufälligen partner, der schon reproduced hat
				vector<vector<int>> partner_nodes_reproduced;
				for (int i = x_edge; i < x_edge + 5; i++) {
					for (int n = y_edge; n < y_edge + 5; n++) {
						if (i > (width_values.size() - 1) || n > (height_values.size() - 1)) continue;
						if (i == x && n == y) continue;
						
						vector<int> pos(2);
						pos.at(0) = i;
						pos.at(1) = n;
						partner_nodes_reproduced.push_back(pos);
					}
				}
				random_shuffle(partner_nodes_reproduced.begin(), partner_nodes_reproduced.end());
				
				x_partner = partner_nodes_reproduced.at(0).at(0);
				y_partner = partner_nodes_reproduced.at(0).at(1);
				
				
			} else {
				
				x_partner = partner_nodes.at(0).at(0);
				y_partner = partner_nodes.at(0).at(1);
				
			}
			
			
			// kind1 bey2
			new_node_map.at(y).at(x).bey2[0] = node_map.at(y).at(x).bey2[rand() % 2];
			new_node_map.at(y).at(x).bey2[1] = node_map.at(y_partner).at(x_partner).bey2[rand() % 2];
			
			//kind2 bey2
			new_node_map.at(y_partner).at(x_partner).bey2[0] = node_map.at(y).at(x).bey2[rand() % 2];
			new_node_map.at(y_partner).at(x_partner).bey2[1] = node_map.at(y_partner).at(x_partner).bey2[rand() % 2];
			
			//kind1 gey
			new_node_map.at(y).at(x).gey[0] = node_map.at(y).at(x).gey[rand() % 2];
			new_node_map.at(y).at(x).gey[1] = node_map.at(y_partner).at(x_partner).gey[rand() % 2];
			
			//kind2 gey
			new_node_map.at(y_partner).at(x_partner).gey[0] = node_map.at(y).at(x).gey[rand() % 2];
			new_node_map.at(y_partner).at(x_partner).gey[1] = node_map.at(y_partner).at(x_partner).gey[rand() % 2];
			
			
			node_map.at(y).at(x).reproduced = true;
			node_map.at(y_partner).at(x_partner).reproduced = true;
			
		}
	}
	
	return new_node_map;
}

void draw_raster(Mat &mat) {
	if (node_size < 3) return;
	for (int y = 0; y < height/node_size; y++) {
		line(mat, Point(0, y * node_size), Point(width, y * node_size), Scalar(0x5c, 0x5c, 0x5c));
	}
	
	for (int x = 0; x < width/node_size; x ++) {
		line(mat, Point(x * node_size, 0), Point(x * node_size, height), Scalar(0x5c, 0x5c, 0x5c));
	}
	
	
}

void draw_node_map(vector<vector<NODE> > &node_map, Mat &mat) {
	for (int y = 0; y < node_map.size(); y++) {
		for (int x = 0; x < node_map.at(y).size(); x++) {
			
			Rect r;
			r.y = y * node_size;
			r.x = x * node_size;
			r.width = node_size;
			r.height = node_size;
			
			if (node_map.at(y).at(x).color == "braun") {
				
				rectangle(mat, r, Scalar(0x00, 0x3F, 0x7B), CV_FILLED);
				
			} else if (node_map.at(y).at(x).color == "blau") {
			
				rectangle(mat, r, Scalar(0xFF, 0x76, 0x48), CV_FILLED);
				
			} else if (node_map.at(y).at(x).color == "gruen") {
			
				rectangle(mat, r, Scalar(0x76, 0xEE, 0x00), CV_FILLED);
				
			} else if (node_map.at(y).at(x).color == "grau") {
			
				rectangle(mat, r, Scalar(0x99, 0x99, 0x99), CV_FILLED);
			}
		}
	}
}

void determine_color(vector<vector<NODE> > &node_map) {
	for(vector<vector<NODE> >::iterator x = node_map.begin(); x != node_map.end(); x++) {
		for (vector<NODE>::iterator y = x->begin(); y != x->end(); y++) {
			if (y->bey2[0] == "braun" || y->bey2[1]  == "braun") {
				y->color = "braun";
				
			} else if (y->gey[0] == "gruen" || y->gey[1] == "gruen"){
				y->color = "gruen";
				
			} else if (y->gey[0] == "blau" || y->gey[1] == "blau"){
				y->color = "blau";
				
			} else {
				y->color = "grau";
			}
		}
	}
}


void preload_init_node_map(vector<vector<NODE> > &node_map, string path) {
	
	Mat mat = imread(path, CV_LOAD_IMAGE_COLOR);
	
	//cout << mat.size().width << " " << mat.size().height << endl;
	
	for (int y = 0; y < mat.size().height; y++) {
		for (int x = 0; x < mat.size().width; x++) {
			
			Vec3b p_bgr = mat.at<Vec3b>(x, y);
			
			if (p_bgr[0] == 0) {
				//braun
				node_map.at(y).at(x).color = "braun";
				node_map.at(y).at(x).bey2[0] = "braun";
				node_map.at(y).at(x).bey2[1] = "braun";
				
				/*
				int b = rand() % 100;
				if (b < 34) {
					node_map.at(y).at(x).gey[0] = "blau";
					
				} else if (b < 67) {
					node_map.at(y).at(x).gey[0] = "gruen";
					
				} else if (b < 100) {
					node_map.at(y).at(x).gey[0] = "grau";
				}
				
				b = rand() % 100;
				if (b < 34) {
					node_map.at(y).at(x).gey[1] = "blau";
					
				} else if (b < 67) {
					node_map.at(y).at(x).gey[1] = "gruen";
					
				} else if (b < 100) {
					node_map.at(y).at(x).gey[1] = "grau";
				}
				*/
				node_map.at(y).at(x).gey[0] = "blau";
				node_map.at(y).at(x).gey[1] = "blau";
				
			} else if(p_bgr[0] == 255) {
				//blau
				node_map.at(y).at(x).color = "blau";
				node_map.at(y).at(x).bey2[0] = "blau";
				node_map.at(y).at(x).bey2[1] = "blau";
				
				node_map.at(y).at(x).gey[0] = "blau";
				node_map.at(y).at(x).gey[1] = "blau";
				//node_map.at(y).at(x).gey[1] = rand() % 100 < 50 ? "blau" : "grau";
			
			} else if(p_bgr[0] == 118) {
				//gruen
				node_map.at(y).at(x).color = "gruen";
				node_map.at(y).at(x).bey2[0] = "blau";
				node_map.at(y).at(x).bey2[1] = "blau";
				
				node_map.at(y).at(x).gey[0] = "gruen";
				node_map.at(y).at(x).gey[1] = "gruen";
				
				/*
				int b = rand() % 100;
				if (b < 34) {
					node_map.at(y).at(x).gey[1] = "blau";
					
				} else if (b < 67) {
					node_map.at(y).at(x).gey[1] = "gruen";
					
				} else if (b < 100) {
					node_map.at(y).at(x).gey[1] = "grau";
				}
				*/
			
			} else if(p_bgr[0] == 153) {
				//grau
				node_map.at(y).at(x).color = "grau";
				node_map.at(y).at(x).bey2[0] = "grau";
				node_map.at(y).at(x).bey2[1] = "grau";
				
				node_map.at(y).at(x).gey[0] = "grau";
				node_map.at(y).at(x).gey[1] = "grau";
				
			}
			
		}
	}
	
}

void random_init_node_map(vector<vector<NODE> > &node_map) {
	for(vector<vector<NODE> >::iterator x = node_map.begin(); x != node_map.end(); x++) {
		for (vector<NODE>::iterator y = x->begin(); y != x->end(); y++) {
		
			y->bey2[0] = rand() % 100 < 50 ? "braun" : "blau";
			y->bey2[1] = rand() % 100 < 50 ? "braun" : "blau";
			
			int b = rand() % 100;
			if (b < 34) {
				y->gey[0] = "blau";
				
			} else if (b < 67) {
				y->gey[0] = "gruen";
				
			} else if (b < 100) {
				y->gey[0] = "grau";
			}
			
			b = rand() % 100;
			if (b < 34) {
				y->gey[1] = "blau";
				
			} else if (b < 67) {
				y->gey[1] = "gruen";
				
			} else if (b < 100) {
				y->gey[1] = "grau";
			}
			
		}
	}
	determine_color(node_map);
}



