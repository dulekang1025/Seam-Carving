#define SEAMCARVINGCOMP665156

#include <opencv2/opencv.hpp>


bool seam_carving(cv::Mat& in_image, int new_width, int new_height, cv::Mat& out_image);
bool generate_energy_to_bigvec_horizontal(cv::Mat img, int i_bound, int j_bound);
bool generate_energy_to_bigvec_vertical(cv::Mat img, int i_bound, int j_bound);
bool reduce_horizontal_seam_trivial(cv::Mat& in_image, cv::Mat& out_image);
bool reduce_vertical_seam_trivial(cv::Mat& in_image, cv::Mat& out_image);
bool seam_carving_trivial(cv::Mat& in_image, int new_width, int new_height, cv::Mat& out_image);
bool find_horizontal_min_energy_seam(cv::Mat in_image, int current_height, int current_width);
bool find_vertical_min_energy_seam(cv::Mat in_image, int current_height, int current_width);
bool save_route_horizontal(int i, int j, int i_bound, int j_bound);
bool save_route_vertical(int i, int j, int i_bound, int j_bound);
