#include "feature_preprocess.h"

void save_allSURFDescriptions_YML(bool greyscale) {
	int db_id = 0;
	Mat db_img;
	FILE* fp;
	fopen_s(&fp, IMAGE_LIST_FILE, "r");
	printf("Extracting Descriptions from input images...\n");

	while (read_images(fp, db_img, db_id)) {
		if (greyscale) {
			Mat greyMat;
			cvtColor(db_img, greyMat, CV_BGR2GRAY);
			db_img = greyMat;
		}
		Mat descriptors = calSURFDescriptor(db_img);

		string foldername = greyscale ? "../g_surf/" : "../surf/";
		FileStorage fs(foldername + to_string(db_id) + ".yml", FileStorage::WRITE);
		fs << "descriptors" << descriptors;

		fs.release();

		db_id++;
	}
}

void save_allSIFTDescriptions_YML(bool greyscale) {
	int db_id = 0;
	Mat db_img;
	FILE* fp;
	fopen_s(&fp, IMAGE_LIST_FILE, "r");
	printf("Extracting Descriptions from input images...\n");

	while (read_images(fp, db_img, db_id)) {
		if (greyscale) {
			Mat greyMat;
			cvtColor(db_img, greyMat, CV_BGR2GRAY);
			db_img = greyMat;
		}
		Mat descriptors = calSIFTDescriptor(db_img);

		string foldername = greyscale ? "../g_sift/" : "../sift/";
		FileStorage fs(foldername + to_string(db_id) + ".yml", FileStorage::WRITE);
		fs << "descriptors" << descriptors;

		fs.release();

		db_id++;
	}
}

void save_allORBDescriptions_YML(bool greyscale) {
	int db_id = 0;
	Mat db_img;
	FILE* fp;
	fopen_s(&fp, IMAGE_LIST_FILE, "r");
	printf("Extracting Descriptions from input images...\n");

	while (read_images(fp, db_img, db_id)) {
		if (greyscale) {
			Mat greyMat;
			cvtColor(db_img, greyMat, CV_BGR2GRAY);
			db_img = greyMat;
		}
		Mat descriptors = calORBDescriptor(db_img);

		string foldername = greyscale ? "../g_orb/" : "../orb/";
		FileStorage fs(foldername + to_string(db_id) + ".yml", FileStorage::WRITE);
		fs << "descriptors" << descriptors;

		fs.release();

		db_id++;
	}
}