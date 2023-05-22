#include<iostream>
#include<fstream>
#include<vector>
using namespace std;

// Загрузка данных из файла проводится в виде - phi \t theta \t M
// RA - phi Dec - theta

struct angle {
	double phi = 0; // xy
	double theta = 0; // z
	double M = 0; // apparent magnitude
};

ostream& operator << (ostream& out, const angle& a)  // выходной поток (перегрузка)
{
	out << a.phi << "\t" << a.theta << endl;
	return out;
}

istream& operator >> (ifstream& in, angle& a) // перегрузка извлечения из потока
{
	in >> a.phi >> a.theta >> a.M;
	return in;
}

angle Fun1(const vector<angle>& a) {  // без учета vis.mag
	double tempT = 0, tempP = 0;
	for (int i = 0; i < a.size(); i++) {
		tempT += a[i].phi;
		tempP += a[i].theta;
	}
	angle result;
	result.phi = tempT / a.size();
	result.theta = tempP / a.size();
	if (result.phi < 0) result.phi += 360;
	return result;
}

angle Fun2(const vector<angle>& a) { // с учетом vis.mag
	double tempT = 0, tempP = 0, tempM = 0;
	for (int i = 0; i < a.size(); i++) {
		tempT += a[i].phi * a[i].M;
		tempP += a[i].theta * a[i].M;
		tempM += a[i].M;
	}
	angle result;
	result.phi = tempT / tempM;
	result.theta = tempP / tempM;
	if (result.phi < 0) result.phi += 360;
	return result;
}

int main()
{
	vector<string> name;
	string tempName;
	ifstream finName{ "filenames.txt" };
	if (!finName) { cerr << "File not found"; return 1; }
	while (!finName.eof()) {
		finName >> tempName;
		name.push_back(tempName);
	}

	ofstream foutRes{ "resultEfficiency.csv" };
	foutRes << "Name,Efficiency phi,Efficiency theta\n";
	ofstream foutRes1{ "resulMain_withoutVisMag.csv" };
	foutRes1 << "Name,RA(phi),Dec(theta)\n";
	ofstream foutRes2{ "resulMain_withVisMag.csv" };
	foutRes2 << "Name,RA(phi),Dec(theta)\n";
	for (int i = 0; i < name.size(); i++)
	{
		vector<angle> a;
		angle temp;
		int index = 0;
		ifstream fin{ "Data//List_of_stars_in_" + name[i] + ".csv" };
		if (!fin) { cerr << "File not found"; return 1; }

		while (!fin.eof()) {
			fin >> temp;
			a.push_back(temp);
			if (a[index].phi >= 200) a[index].phi -= 360;
			a[index].M *= -10;
			a[index].M += 160;
			index++;
		}
		angle r1 = Fun1(a);
		angle r2 = Fun2(a);
		cout << i + 1 << " " << name[i] << "\t\tEfficiency phi: " << (((r2.phi - r1.phi) / r2.phi) * 100) << "%;\tEfficiency theta: " << (((r2.theta - r1.theta) / r2.theta) * 100) << "%\n";
		foutRes << name[i] << "," << (((r2.phi - r1.phi) / r2.phi) * 100) << "," << (((r2.theta - r1.theta) / r2.theta) * 100) << "\n";
		foutRes1 << name[i] << "," << r1.phi << "," << r1.theta << "\n";
		foutRes2 << name[i] << "," << r2.phi << "," << r2.theta << "\n";
	}

}