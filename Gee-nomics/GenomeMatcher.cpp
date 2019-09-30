#include "provided.h"
#include "Trie.h"
#include <string>
#include <list>
#include <vector>
#include <unordered_map>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

class GenomeMatcherImpl
{
public:
	GenomeMatcherImpl(int minSearchLength);
	void addGenome(const Genome& genome);
	int minimumSearchLength() const;
	bool findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const;
	bool findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const;
private:
	string intToString(int i);
	int m_min;
	Trie<string> m_trie;
	vector<Genome> m_genomes;
	// unordered_map<string, int> m_namesToLoc;
	void getInfo(const string s, int& num, int& pos) const;
	void DNAComp(DNAMatch& d1, int length, int pos) const;
	void doStuff(const string& f, vector<string>& g, vector<DNAMatch>& d, string temp, string n, int pos, int minLen, bool exactMatch) const;
	bool GMComp(GenomeMatch& g1, GenomeMatch& g2) const;
	void heapify(vector<GenomeMatch>& v, int n, int i) const;
	void heapSort(vector<GenomeMatch>& v) const;
};

GenomeMatcherImpl::GenomeMatcherImpl(int minSearchLength)
{
	m_min = minSearchLength;
	// This compiles, amd may be correct
}

string GenomeMatcherImpl::intToString(int i) {
	ostringstream oss;
	oss << i;
	return oss.str();
}

void GenomeMatcherImpl::addGenome(const Genome& genome)
{
	// m_namesToLoc.emplace(genome.name(), m_genomes.size());
	m_genomes.push_back(genome);
	for (int i = 0; i <= genome.length() - m_min; i++) {
		string tempSeq;
		string frag;
		if (genome.extract(i, m_min, tempSeq)) {
			frag = "Genome ";
			frag += intToString(m_genomes.size());
			// frag += m_genome.name();
			frag += ", position ";
			frag += intToString(i);
			m_trie.insert(tempSeq, frag);
		}
	}
	// This compiles, amd may be correct
}

int GenomeMatcherImpl::minimumSearchLength() const
{
	return m_min;
	// This compiles, amd may be correct
}

// private function to get specific data
void GenomeMatcherImpl::getInfo(const string s, int& num, int& pos) const {
	bool isNum = true;
	num = 0;
	pos = 0;
	for (int i = 0; i < s.size(); i++) {
		if (isdigit(s[i]) && isNum) {
			num *= 10;
			num += s[i] - '0';
		}
		else if (isdigit(s[i])) {
			pos *= 10;
			pos += s[i] - '0';
		}
		if (s[i] == ',') {
			isNum = false;
		}
	}
}

// puts greater values of the two DNAMatch objects in d1
void GenomeMatcherImpl::DNAComp(DNAMatch& d1, int length, int position) const {
	// d1 is shorter than d2, so update d1
	if (d1.length < length) {
		d1.position = position;
		d1.length = length;
		return;
	}
	// d1 is later than d2, so update d1
	if (d1.length == length) {
		if (d1.position > position) {
			d1.position = position;
			// and length is the same
		}
	}
}

// 
void GenomeMatcherImpl::doStuff(const string& f, vector<string>& g, vector<DNAMatch>& d, string temp, string n, int pos, int minLen, bool exactMatch) const {
	// get the length of longest matching substring
	int length = 0;
	for (int j = 0; j < temp.size(); j++) {
		if (temp[j] == f[j]) {
			length++;
		}
		// allow for one inexact match
		else if (!exactMatch) {
			length++;
			exactMatch = true;
		}
		else {
			break;
		}
	}
	// don't continue if this string is less than minimum length
	if (length < minLen) {
		return;
	}
	bool alreadyAdded = false;
	// if current genome is already in the vector, keep the longer/earlier one
	for (int j = 0; j < d.size(); j++) {
		if (d[j].genomeName == n) {
			DNAComp(d[j], length, pos);
			alreadyAdded = true;
			break;
		}
	}
	// if current genome does not exist in vector, make a new DNAMatch object
	if (!alreadyAdded) {
		DNAMatch newDNA;
		newDNA.genomeName = n;
		newDNA.length = length;
		newDNA.position = pos;
		d.push_back(newDNA);
	}
}

bool GenomeMatcherImpl::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
	// fragment's length is less than minlength or minlength is less than minSearchLength
	if (minimumLength < m_min || fragment.size() < minimumLength) {
		// debugging purposes
		// cerr << "result: false; matches: none " << endl;
		return false;
	}
	vector<string> g;
		g = m_trie.find(fragment.substr(0, m_min), exactMatchOnly);
	matches.clear();
	for (int i = 0; i < g.size(); i++) {
		int num, pos;
		getInfo(g[i], num, pos);
		string temp;
		string n = m_genomes[num - 1].name();
		// if successfully extract fragment length string from genome
		if (m_genomes[num - 1].extract(pos, fragment.size(), temp)) {
			doStuff(fragment, g, matches, temp, n, pos, minimumLength, exactMatchOnly);
		}
		// else extract as much of the string as possible
		else if (m_genomes[num - 1].extract(pos, m_genomes[num - 1].length() - pos, temp)) {
			doStuff(fragment, g, matches, temp, n, pos, minimumLength, exactMatchOnly);
		}
		// soething went wrong? -_-
		else {
			return false;
		}
	}
	// no matches
	if (matches.size() == 0) {
		// debugging purposes
		// cerr << "result: false; matches: none " << endl;
		return false;
	}
	/*
	// debugging purposes
	cerr << "result: true; matches: " << endl;
	for (int i = 0; i < matches.size(); i++) {
		cerr << "Genome " << matches[i].genomeName << " of length " << matches[i].length
			<< " at position " << matches[i].position << endl;
	}
	*/
	return true;  // This compiles, but may not be correct
}

// returns true if g1 > g2
bool GenomeMatcherImpl::GMComp(GenomeMatch& g1, GenomeMatch& g2) const {
	if (g1.percentMatch > g2.percentMatch) {
		return true;
	}
	if (g1.percentMatch < g2.percentMatch) {
		return false;
	}
	if (g1.genomeName < g2.genomeName) {
		return true;
	}
	return false;
}


// https://www.geeksforgeeks.org/heap-sort/
void GenomeMatcherImpl::heapify(vector<GenomeMatch>& v, int n, int i) const {
	int largest = i; // Initialize largest as root 
	int l = 2 * i + 1; // left = 2*i + 1 
	int r = 2 * i + 2; // right = 2*i + 2 

	// If left child is larger than root 
	if (l < n && GMComp(v[l], v[largest]))
		largest = l;

	// If right child is larger than largest so far 
	if (r < n && GMComp(v[r], v[largest]))
		largest = r;

	// If largest is not root 
	if (largest != i)
	{
		int p = v[largest].percentMatch;
		v[largest].percentMatch = v[i].percentMatch;
		v[i].percentMatch = p;
		string name = v[largest].genomeName;
		v[largest].genomeName = v[i].genomeName;
		v[i].genomeName = name;

		// Recursively heapify the affected sub-tree 
		heapify(v, n, largest);
	}
}

// https://www.geeksforgeeks.org/heap-sort/
void GenomeMatcherImpl::heapSort(vector<GenomeMatch>& v) const {
	for (int i = v.size() / 2 - 1; i >= 0; i--) {
		heapify(v, v.size(), i);
	}
	for (int i = v.size() - 1; i >= 0; i--) {
		int p = v[0].percentMatch;
		v[0].percentMatch = v[i].percentMatch;
		v[i].percentMatch = p;
		string n = v[0].genomeName;
		v[0].genomeName = v[i].genomeName;
		v[i].genomeName = n;

		heapify(v, i, 0);
	}
}

bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
	results.clear();
	// why of all things does DNAMatch use the name?
	unordered_map<string, int> score;
	set<string> names;
	int S = query.length() / fragmentMatchLength;
	for (int i = 0; i < S; i++) {
		string frag;
		// don't see why this would ever be false, but just in case
		if (query.extract(i*fragmentMatchLength, fragmentMatchLength, frag)) {
			vector<DNAMatch> matches;
			findGenomesWithThisDNA(frag, fragmentMatchLength, false, matches);
			for (int j = 0; j < matches.size(); j++) {
				// int loc = m_namesToLoc.at(matches[i].genomeName);
				// apparently ints are initialized to 0, yay
				score[matches[j].genomeName]++;
				names.insert(matches[j].genomeName);
			}
		}
	}
	// no matches found :(
	if (names.size() == 0) {
		return false;
	}
	// else smack all the items in
	set<string>::iterator it = names.begin();
	while (it != names.end()) {
		int p = score[*it] * 100 / S;
		if (p > matchPercentThreshold) {
			GenomeMatch g;
			g.genomeName = *it;
			g.percentMatch = p;
			results.push_back(g);
		}
		it++;
	}
	// and heap sort i guess
	heapSort(results);
	
	return true;  // This compiles, and may not be correct TT^TT
}

//******************** GenomeMatcher functions ********************************

// These functions simply delegate to GenomeMatcherImpl's functions.
// You probably don't want to change any of this code.

GenomeMatcher::GenomeMatcher(int minSearchLength)
{
	m_impl = new GenomeMatcherImpl(minSearchLength);
}

GenomeMatcher::~GenomeMatcher()
{
	delete m_impl;
}

void GenomeMatcher::addGenome(const Genome& genome)
{
	m_impl->addGenome(genome);
}

int GenomeMatcher::minimumSearchLength() const
{
	return m_impl->minimumSearchLength();
}

bool GenomeMatcher::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
	return m_impl->findGenomesWithThisDNA(fragment, minimumLength, exactMatchOnly, matches);
}

bool GenomeMatcher::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
	return m_impl->findRelatedGenomes(query, fragmentMatchLength, exactMatchOnly, matchPercentThreshold, results);
}
