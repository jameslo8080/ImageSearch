#include "feature_preprocess.h"

void save_allSURFDescriptions_YML() {
	int db_id = 0;
	Mat db_img;
	FILE* fp;
	fopen_s(&fp, IMAGE_LIST_FILE, "r");
	printf("Extracting Descriptions from input images...\n");

	while (read_images(fp, db_img, db_id)) {
		SurfFeatureDetector detector;
		SurfDescriptorExtractor extractor;
		vector<KeyPoint> keyPoints;
		Mat descriptors;
		detector.detect(db_img, keyPoints);
		extractor.compute(db_img, keyPoints, descriptors);

		FileStorage fs("../surf/" + to_string(db_id) + ".yml", FileStorage::WRITE);
		fs << "keypoints" << keyPoints;
		fs << "descriptors" << descriptors;

		fs.release();

		db_id++;
	}
}

void save_allSIFTDescriptions_YML() {
	int db_id = 0;
	Mat db_img;
	FILE* fp;
	fopen_s(&fp, IMAGE_LIST_FILE, "r");
	printf("Extracting Descriptions from input images...\n");

	while (read_images(fp, db_img, db_id)) {
		SiftFeatureDetector  sift;
		vector<KeyPoint> keyPoints;
		Mat descriptors;
		sift.detect(db_img, keyPoints);
		sift.compute(db_img, keyPoints, descriptors);

		FileStorage fs("../sift/" + to_string(db_id) + ".yml", FileStorage::WRITE);
		fs << "keypoints" << keyPoints;
		fs << "descriptors" << descriptors;

		fs.release();

		db_id++;
	}
}

void save_allORBDescriptions_YML() {
	int db_id = 0;
	Mat db_img;
	FILE* fp;
	fopen_s(&fp, IMAGE_LIST_FILE, "r");
	printf("Extracting Descriptions from input images...\n");

	while (read_images(fp, db_img, db_id)) {

		OrbFeatureDetector detector;
		OrbDescriptorExtractor extractor;
		vector<KeyPoint> keyPoints;
		Mat descriptors;
		detector.detect(db_img, keyPoints);
		extractor.compute(db_img, keyPoints, descriptors);

		FileStorage fs("../orb/" + to_string(db_id) + ".yml", FileStorage::WRITE);
		fs << "keypoints" << keyPoints;
		fs << "descriptors" << descriptors;

		fs.release();

		db_id++;
	}
}