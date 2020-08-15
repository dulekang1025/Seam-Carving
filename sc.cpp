
#include "sc.h"
using namespace cv;
using namespace std;



static vector< vector<int> > bigvec;
static map<pair<int, int>, bool> mapNeedToDelete;
static vector<int> horizontal_changed_point;
static vector<int> vertical_changed_point;

bool seam_carving(Mat& in_image, int new_width, int new_height, Mat& out_image) {
	cout << "enter seam_carving" << endl;
	// some sanity checks
	// Check 1 -> new_width <= in_image.cols
	if (new_width>in_image.cols) {
		cout << "Invalid request!!! new_width has to be smaller than the current size!" << endl;
		return false;
	}
	if (new_height>in_image.rows) {
		cout << "Invalid request!!! ne_height has to be smaller than the current size!" << endl;
		return false;
	}

	if (new_width <= 0) {
		cout << "Invalid request!!! new_width has to be positive!" << endl;
		return false;

	}

	if (new_height <= 0) {
		cout << "Invalid request!!! new_height has to be positive!" << endl;
		return false;

	}

	return seam_carving_trivial(in_image, new_width, new_height, out_image);
}

bool seam_carving_trivial(Mat& in_image, int new_width, int new_height, Mat& out_image) {
    //cout << "seam_carving_trivial" << endl;
    int height = 0, col = 0;

    Mat iimage = in_image.clone();
    Mat oimage = in_image.clone();
    while (iimage.rows != new_height || iimage.cols != new_width) {
        // horizontal seam if needed
        if (iimage.rows>new_height) {
            cout << ++height << endl;
            reduce_horizontal_seam_trivial(iimage, oimage);
            iimage = oimage.clone();
        }

        if (iimage.cols>new_width) {
            cout << ++col << endl;
            reduce_vertical_seam_trivial(iimage, oimage);
            iimage = oimage.clone();
        }
    }
    cout << "before clone" << endl;
    //////system("pause");
    out_image = oimage.clone();
    cout << "after clone" << endl;
    //////system("pause");


    return true;
}

bool reduce_horizontal_seam_trivial(Mat& in_image, Mat& out_image) {

	int rows = in_image.rows - 1;
	int cols = in_image.cols;

	out_image = Mat(rows, cols, CV_8UC3);

	int middle = in_image.rows / 2;

	if (!find_horizontal_min_energy_seam(in_image, in_image.rows, in_image.cols)) {
		cout << "CANNOT FIND" << endl;
	}
	else {
		cout << "we can start to cut!!!!!!!!!" << endl;
	}
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {

			if (i >= horizontal_changed_point[j]) {//pixel (need to delete) and folowing pixels should move 1 unit
				Vec3b pixel = in_image.at<Vec3b>(i + 1, j); //copy the bottom pixel
				out_image.at<Vec3b>(i, j) = pixel;
			}
			else {
				Vec3b pixel = in_image.at<Vec3b>(i, j); //copy directly
				out_image.at<Vec3b>(i, j) = pixel;
			}
		}

	}
	return true;
}

// vertical trivial seam is a seam through the center of the image
bool reduce_vertical_seam_trivial(Mat& in_image, Mat& out_image) {
	int rows = in_image.rows;
	int cols = in_image.cols - 1;

	out_image = Mat(rows, cols, CV_8UC3);

	int middle = in_image.cols / 2;

	if (!find_vertical_min_energy_seam(in_image, in_image.rows, in_image.cols)) {
		cout << "CANNOT FIND" << endl;
	}
	else {
		cout << "we can start to cut vertical~~~" << endl;
	}

	for (int j = 0; j<cols; ++j)
		for (int i = 0; i < rows; ++i) {

			if (j >= vertical_changed_point[i]) {//pixel (need to delete) and folowing pixels should move 1 unit
				Vec3b pixel = in_image.at<Vec3b>(i, j+1); //copy the bottom pixel
				out_image.at<Vec3b>(i, j) = pixel;
			}
			else {
				Vec3b pixel = in_image.at<Vec3b>(i, j); //copy directly
				out_image.at<Vec3b>(i, j) = pixel;
			}
		}

	return true;
}


bool find_horizontal_min_energy_seam(Mat in_image,int current_height, int current_width) {
	cout << "find_horizontal_min_energy_seam" << endl;
	horizontal_changed_point.resize(current_width);// 1 unit bigger than expected 
	Mat gray_backup = in_image.clone();
	Mat1b gray;
	cvtColor(gray_backup, gray, COLOR_BGR2GRAY);
	
	//below is test part2
	Mat src, src_gray;
	Mat grad;
	char* window_name = "Sobel Demo - Simple Edge Detector";
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	/// Load an image
	src = gray_backup;

	if (!src.data)
	{		return false;	}

	GaussianBlur(src, src, Size(3, 3), 0, 0, BORDER_DEFAULT);
	/// Convert it to gray
	cvtColor(src, src_gray, COLOR_RGB2GRAY);
	/// Create window
	namedWindow(window_name, WINDOW_AUTOSIZE);
	/// Generate grad_x and grad_y
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;
	/// Gradient X
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel(src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);
	/// Gradient Y
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel(src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_y, abs_grad_y);
	addWeighted(abs_grad_x, 0.3, abs_grad_y, 0.1, 0, grad);//if find horizontal seam, higher x is better
	imshow(window_name, grad);
	waitKey(100);

	

	bigvec.clear();
	if (bigvec.empty()) {

	}
	bigvec.resize(gray_backup.rows, vector<int>(gray_backup.cols,999999));

	horizontal_changed_point.clear();
	horizontal_changed_point.resize(current_width);
	generate_energy_to_bigvec_horizontal(grad, current_height, current_width);
	int minimum = 999999, minimumLine;
	for (int k = 0; k < current_height; ++k) {	//find minimum energy seam
		if (bigvec[k][0] < minimum) {
			
			minimum = bigvec[k][0];
			minimumLine = k;
			horizontal_changed_point[0] = minimumLine;
		}
	}

	if(save_route_horizontal(minimumLine, 0, current_height, current_width)) {
		for (int t = 0; t < current_width; ++t) {
		}
		cout << "save_route finished" << endl;
	}
	else {
		cout << "save_route failed" << endl;
	}

}










bool find_vertical_min_energy_seam(Mat in_image, int current_height, int current_width) {
	cout << "find_vertical_min_energy_seam" << endl;
	vertical_changed_point.resize(current_height);// 1 unit bigger than expected 
	Mat gray_backup = in_image.clone();
	Mat1b gray;
	cvtColor(gray_backup, gray, COLOR_BGR2GRAY);
	//cout << "22222222222222222" << endl;

	//below is test part2
	Mat src, src_gray;
	Mat grad;
	char* window_name = "Sobel Demo - Simple Edge Detector";
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	/// Load an image
	src = gray_backup;
	//cout << "3333333333333333333333" << endl;

	if (!src.data)
	{
		return false;
	}

	GaussianBlur(src, src, Size(3, 3), 0, 0, BORDER_DEFAULT);
	/// Convert it to gray
	cvtColor(src, src_gray, COLOR_RGB2GRAY);
	/// Create window
	namedWindow(window_name, WINDOW_AUTOSIZE);
	/// Generate grad_x and grad_y
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;
	/// Gradient X
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel(src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);
	/// Gradient Y
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel(src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_y, abs_grad_y);
	addWeighted(abs_grad_x, 0.1, abs_grad_y, 0.2, 0, grad);//if find vertical seam, higher y is higher
	imshow(window_name, grad);
	waitKey(100);

	Mat1f magn;//maybe useless


	bigvec.clear();
	if (bigvec.empty()) {

	}
	bigvec.resize(gray_backup.rows, vector<int>(gray_backup.cols, 999999));

	if (bigvec.empty()) {
		cout << "empty empty" << endl;

	}

	else {

	}
	vertical_changed_point.clear();
	vertical_changed_point.resize(current_height);
	generate_energy_to_bigvec_vertical(grad, current_height, current_width);


	int minimum = 999999, minimumCol;
	for (int k = 0; k < current_width; ++k) {	//find minimum energy seam
		if (bigvec[0][k] < minimum) {

			minimum = bigvec[0][k];
			minimumCol = k;
			vertical_changed_point[0] = minimumCol;
		}
	}

	cout << "vertical_changed_point[0] is " << vertical_changed_point[0] << endl;

	if (save_route_vertical(0, minimumCol, current_height, current_width)) {
		for (int t = 0; t < current_height; ++t) {
		}
	}
	else {
	}

}



bool save_route_horizontal(int i, int j, int i_bound, int j_bound) {
	int upperLine, sameLine, lowerLine;
	if (j + 1 >= j_bound || j <-1) {
		return true;
	}
	else if (i == 0) {
		upperLine = 999999;
		sameLine = bigvec[i][j + 1];
		lowerLine = bigvec[i + 1][j + 1];
	}
	else if (i == i_bound - 1) {
		upperLine = bigvec[i - 1][j + 1];
		sameLine = bigvec[i][j + 1];
		lowerLine = 999999;
	}
	else {
		upperLine = bigvec[i - 1][j + 1];
		sameLine = bigvec[i][j + 1];
		lowerLine = bigvec[i + 1][j + 1];
	}


	if (upperLine <= sameLine && upperLine <= lowerLine) {
		horizontal_changed_point[j + 1] = i - 1;
		save_route_horizontal(i - 1, j + 1, i_bound, j_bound);
	}
	else if (sameLine <= upperLine && sameLine <= lowerLine) {
		horizontal_changed_point[j + 1] = i;
		save_route_horizontal(i, j + 1, i_bound, j_bound);
	}
	else {
		horizontal_changed_point[j + 1] = i + 1;
		save_route_horizontal(i + 1, j + 1, i_bound, j_bound);
	}

}

bool save_route_vertical(int i, int j, int i_bound, int j_bound) {
	//cout << "enter save_route_vertical"  << endl;
	int lastCol, sameCol, nextCol;
	if (i + 1 >= i_bound || 1 <-1) {
		return true;
	}
	else if (j == 0) {
		lastCol = 999999;
		sameCol = bigvec[i + 1][j];
		nextCol = bigvec[i + 1][j + 1];
	}
	else if (j == j_bound - 1) {
		lastCol = bigvec[i + 1][j - 1];
		sameCol = bigvec[i + 1][j];
		nextCol = 999999;
	}
	else {
		lastCol = bigvec[i + 1][j - 1];
		sameCol = bigvec[i + 1][j];
		nextCol = bigvec[i + 1][j + 1];
	}
		//cout << "9999999999999999" << endl;


	if (lastCol <= sameCol && lastCol <= nextCol) {
		vertical_changed_point[i + 1] = j - 1;
		save_route_vertical(i + 1, j - 1, i_bound, j_bound);
	}
	else if (sameCol <= lastCol && sameCol <= nextCol) {
		vertical_changed_point[i + 1] = j;
		save_route_vertical(i + 1, j, i_bound, j_bound);
	}
	else {
		vertical_changed_point[i + 1] = j + 1;
		save_route_vertical(i + 1, j + 1, i_bound, j_bound);
	}

}

bool generate_energy_to_bigvec_horizontal(Mat img, int i_bound, int j_bound) {
	int q = 0, p = 0;
	for (p = j_bound-1; p >= 0; --p) {

		for (q = 0; q < i_bound; ++q) {
			if (bigvec[q][p] >=999999 ) {

				int value1, value2, value3;

				if (p == j_bound-1) {
					value1 = abs((int)img.at<char>(q, p));
					bigvec[q][p] = value1;//put zero to ingore last column
					continue;
				}else if (q == 0) {
					value1 = 999999;
					value2 = abs((int)img.at<char>(q, p)) + abs(bigvec[q][p + 1]);
					value3 = abs((int)img.at<char>(q, p)) + abs(bigvec[q + 1][p + 1]);

				}
				else if (q == i_bound-1) {
					value1 = abs((int)img.at<char>(q, p)) + abs(bigvec[q - 1][p + 1]);
					value2 = abs((int)img.at<char>(q, p)) + abs(bigvec[q][p + 1]);
					value3 = 999999;

				}
				else {
					value1 = abs((int)img.at<char>(q, p)) + abs(bigvec[q - 1][p + 1]);
					value2 = abs((int)img.at<char>(q, p)) + abs(bigvec[q][p + 1]);
					value3 = abs((int)img.at<char>(q, p)) + abs(bigvec[q + 1][p + 1]);

				}

				if (value1 <= value2 && value1 <= value3) {
					bigvec[q][p] = value1;

				}
				else if (value2 <= value1 && value2 <= value3) {
					bigvec[q][p] = value2;

				}
				else {
					bigvec[q][p] = value3;

				}
			}
			else {
				return false;

			}
			
		}
	}
	return true;
}

bool generate_energy_to_bigvec_vertical(Mat img, int i_bound, int j_bound) {
	int q = 0, p = 0;
	for (q = i_bound - 1; q >= 0; --q) {

		for (p = 0; p < j_bound; ++p) {
			if (bigvec[q][p] >= 999999) {

				int value1, value2, value3;

				if (q == i_bound - 1) {
					value1 = abs((int)img.at<char>(q, p));
					bigvec[q][p] = value1;//put zero to ingore last column
					continue;
				}
				else if (p == 0) {
					value1 = 999999;
					value2 = abs((int)img.at<char>(q, p)) + abs(bigvec[q+1][p ]);
					value3 = abs((int)img.at<char>(q, p)) + abs(bigvec[q + 1][p + 1]);

				}
				else if (p == j_bound - 1) {
					value1 = abs((int)img.at<char>(q, p)) + abs(bigvec[q +1][p -1]);
					value2 = abs((int)img.at<char>(q, p)) + abs(bigvec[q+1][p ]);
					value3 = 999999;

				}
				else {
					value1 = abs((int)img.at<char>(q, p)) + abs(bigvec[q +1][p -1]);
					value2 = abs((int)img.at<char>(q, p)) + abs(bigvec[q+1][p ]);
					value3 = abs((int)img.at<char>(q, p)) + abs(bigvec[q + 1][p + 1]);

				}

				if (value1 <= value2 && value1 <= value3) {
					bigvec[q][p] = value1;

				}
				else if (value2 <= value1 && value2 <= value3) {
					bigvec[q][p] = value2;

				}
				else {
					bigvec[q][p] = value3;

				}
			}
			else {
				return false;

			}

		}
	}
	return true;
}

