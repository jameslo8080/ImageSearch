/*
0:'beach', 1:'building', 2:'bus', 3:'dinosaur', 4:'flower', 5:'horse', 6:'man'

1.	compare -> output image#

2.	origin method (1) -> output one image#

3.	origin method (1) -> output >1 image
4.	origin method (1) -> output type of image

4b. 自動出幾多% 岩

Color Layout (or gridded color) distance is the sum of the color distances in each of the corresponding grid squares.


5.	斬 -> output

6.	SURF
feature

7.	斬pattern + method -> 60%

8.  Machine Learning - feature

9.	save d feature

10.  GUI

11.  Report
*/
/*
man			: 0 ~ 99, 100?
beach		: 101 ~ 199, 100?
building	: 200 ~ 299
bus			: 300 ~ 399
dinosaur	: 400 ~ 499
elephant	: 500 ~ 599
flower		: 600 ~ 699
horse		: 700 ~ 799
mountain	: 800 ~ 899
food		: 900 ~ 999
*/

//string files[] = { "beach", "building", "bus", "dinosaur", "flower", "horse", "man" };

// #include <iostream>
// #include <string>
// #include <sstream>
using namespace std;


struct ImageScore {
	int id;
	int score;
	ImageScore() {}
	ImageScore(int i, double s) : id(i), score(s) {}
	bool operator < (const ImageScore & other) const {
		return score < other.score;
	}
};