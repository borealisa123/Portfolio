#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <istream>
using namespace std;

class GenomeImpl
{
public:
    GenomeImpl(const string& nm, const string& sequence);
    static bool load(istream& genomeSource, vector<Genome>& genomes);
    int length() const;
    string name() const;
    bool extract(int position, int length, string& fragment) const;
private:
	string m_name;
	string m_sequence;
	// i don't know? string makes sense here
};

GenomeImpl::GenomeImpl(const string& nm, const string& sequence)
{
	m_name = nm;
	m_sequence = sequence;
	// This compiles, and may be correct
}

bool GenomeImpl::load(istream& genomeSource, vector<Genome>& genomes)
{
	// check if file is valid, probably unecessary
	if (!genomeSource) {
		return false;
	}
	// file is valid
	bool first = true;
	bool preName = false;
	string name;
	string line;
	string sequence;
	// go line by line
	while (getline(genomeSource, line)) {
		if (line.size() == 0) {
			return false;		// empty line
		}
		// name line
		if (line[0] == '>') {
			if (preName) {
				return false;	// name line not followed by base line
			}
			preName = true;
			// add to genome vector
			if (!first) {
				Genome g = Genome(name, sequence);
				genomes.push_back(g);
			}
			name = line.substr(1);
			sequence = "";
		}
		// else base line
		else {
			if (first || line.size() > 80) {
				return false;	// first line is not a name or exceeds char limit
			}
			preName = false;
			for (int i = 0; i < line.size(); i++) {
				switch (toupper(line[i])) {
				case 'A':
					sequence += 'A';
					break;
				case 'C':
					sequence += 'C';
					break;
				case 'T':
					sequence += 'T';
					break;
				case 'G':
					sequence += 'G';
					break;
				case 'N':
					sequence += 'N';
					break;
				default:
					return false;		// invalid char
				}
			}
		}
		first = false;
	}
	if (name.size() > 0 && sequence.size() == 0) {
		return false;				// ended with name line
	}
	// add final genome to vector
	Genome g = Genome(name, sequence);
	genomes.push_back(g);
	return true;
	// This compiles, and may be correct
}

int GenomeImpl::length() const
{
	return m_sequence.size();
	// This compiles, and may be correct
}

string GenomeImpl::name() const
{
	return m_name;
	// This compiles, and may be correct
}

bool GenomeImpl::extract(int position, int length, string& fragment) const
{
	if (position < 0 || position + length > m_sequence.size()) {
		return false;
	}
	fragment = m_sequence.substr(position, length);
	return true;
	// This compiles, and may be correct
}

//******************** Genome functions ************************************

// These functions simply delegate to GenomeImpl's functions.
// You probably don't want to change any of this code.

Genome::Genome(const string& nm, const string& sequence)
{
    m_impl = new GenomeImpl(nm, sequence);
}

Genome::~Genome()
{
    delete m_impl;
}

Genome::Genome(const Genome& other)
{
    m_impl = new GenomeImpl(*other.m_impl);
}

Genome& Genome::operator=(const Genome& rhs)
{
    GenomeImpl* newImpl = new GenomeImpl(*rhs.m_impl);
    delete m_impl;
    m_impl = newImpl;
    return *this;
}

bool Genome::load(istream& genomeSource, vector<Genome>& genomes) 
{
    return GenomeImpl::load(genomeSource, genomes);
}

int Genome::length() const
{
    return m_impl->length();
}

string Genome::name() const
{
    return m_impl->name();
}

bool Genome::extract(int position, int length, string& fragment) const
{
    return m_impl->extract(position, length, fragment);
}
