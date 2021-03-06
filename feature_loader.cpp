#include "feature_loader.h"


bool read_images(FILE* fp, Mat &db_img, int db_id, bool output) {
	char imagepath[200];
	while (!feof(fp))
	{
		while (fscanf_s(fp, "%s ", imagepath, sizeof(imagepath)) > 0)
		{
			if (output)printf("%s, ", imagepath);
			char tempname[200];
			sprintf_s(tempname, 200, "../%s", imagepath);

			db_img = imread(tempname); // read database image
			if (!db_img.data)
			{
				printf("Cannot find the database image number %d!\n", db_id + 1);
				throw "Error";
			}
			return true;
		}
	}
	fclose(fp);
	if (output) printf("\n");
	return false;
}

bool read_images(FILE* fp, Mat &db_img, int db_id) {
	return read_images(fp, db_img, db_id, true);
}

vector<Mat> allDescriptors;
vector<Mat> load_allDescriptions_YML(string type, int partOf100, bool greyscale) {
	if (!allDescriptors.empty()) return allDescriptors;

	FILE* fp;
	char imagepath[200];
	string fpath;
	if (type == "SURF")
		fpath = SURF_LIST_FILE;
	else if (type == "SIFT")
		fpath = SIFT_LIST_FILE;
	else
		fpath = ORB_LIST_FILE;
	if (greyscale) {
		fpath = "g_" + fpath;
	}

	fopen_s(&fp, fpath.c_str(), "r");
	printf("Extracting Descriptions from input images...\n");

	int cnt = -1;
	while (!feof(fp))
	{
		while (fscanf_s(fp, "%s ", imagepath, sizeof(imagepath)) > 0)
		{
			cnt++;
			if (cnt % 100 >= partOf100) continue;

			char tempname[200];
			sprintf_s(tempname, 200, "../%s", imagepath); // ../%s
			cout << "reading : " << tempname << endl;
			FileStorage storage = FileStorage(tempname, FileStorage::READ);

			if (storage["descriptors"].empty()){
				cout << "descriptors is empty" << endl;
			}

			Mat descriptors;

			storage["descriptors"] >> descriptors;
			cout << "descriptors readed" << endl;

			allDescriptors.push_back(descriptors);
			storage.release();
		}
	}
	fclose(fp);

	return allDescriptors;
}

Mat load_allDescriptions() {
	Mat allDescriptors;
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
		allDescriptors.push_back(descriptors);

		db_id++;
	}
	return allDescriptors;
}


vector<Mat> all_hsvHist;
vector<Mat> load_hsvHist() {
	if (!all_hsvHist.empty()) return all_hsvHist;
	///Read Database
	int db_id = 0;
	Mat db_img;
	FILE* fp;
	fopen_s(&fp, IMAGE_LIST_FILE, "r");
	printf("Extracting features from input images...\n");
	while (read_images(fp, db_img, db_id, false)) {
		Mat hist_base = rgbMat_to_hsvHist(db_img);
		all_hsvHist.push_back(hist_base);

		if (db_id % 50 == 0)
			cout << db_id / 10 << "%..";

		db_id++;
	}
	fclose(fp);
	printf("Done\n");
	return all_hsvHist;
}

vector<Mat> all_img;

vector<Mat> load_imgs(bool out) {
	if (!all_img.empty()) return all_img;
	///Read Database
	int db_id = 0;
	Mat db_img;
	FILE* fp;
	fopen_s(&fp, IMAGE_LIST_FILE, "r");
	printf("Loading the image dataset...\n");
	while (read_images(fp, db_img, db_id, out)) {
		all_img.push_back(db_img);
		if (db_id % 50 == 0)
			cout << db_id / 10 << "%..";
		db_id++;
	}
	fclose(fp);
	printf("Done\n");
	return all_img;
}
