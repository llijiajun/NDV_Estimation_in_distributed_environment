#if !defined(HYPERLOGLOG_HPP)
#define HYPERLOGLOG_HPP

#include <vector>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <functional>
#include <iostream>
#include <algorithm>
#include <map>
#include "murmur3_hash.hpp"
#include "Murmurhash.h"


#define HLL_HASH_SEED 313

//#if defined(__has_builtin) && (defined(__GNUC__) || defined(__clang__))

//#define _GET_CLZ(x, b) (uint8_t)std::min(b, ::__builtin_clz(x)) + 1

//#else

inline uint8_t _get_leading_zero_count(uint64_t x, uint8_t b) {

#if defined (_MSC_VER)
	uint32_t leading_zero_len = 64;
	::_BitScanReverse(&leading_zero_len, x);
	--leading_zero_len;
	return std::min(b, (uint8_t)leading_zero_len);
#else
	uint8_t v = 1;
	while (v <= b && !(x & 0x8000000000000000)) {
		v++;
		x <<= 1;
	}
	return v;
#endif

}
#define _GET_CLZ(x, b) _get_leading_zero_count(x, b)
//#endif /* defined(__GNUC__) */
using namespace std;

namespace sketch {
static const double pow_2_32 = 18446744073709551616.0; ///< 2^32
static const double neg_pow_2_32 = -18446744073709551616.0; ///< -(2^32)
/** @class HyperLogLog
 *  @brief Implement of 'HyperLogLog' estimate cardinality algorithm
 */
class HyperLogLog {
public:

	/**
	 * Constructor
	 *
	 * @param[in] b bit width (register size will be 2 to the b power).
	 *			This value must be in the range[4,30].Default value is 4.
	 *
	 * @exception std::invalid_argument the argument is out of range.
	 */
	HyperLogLog(uint8_t b = 4) throw (std::invalid_argument) :
			b_(b), m_(1 << b), M_(m_, 0) {

		if (b < 4 || 30 < b) {
			throw std::invalid_argument("bit width must be in the range [4,30]");
		}

		double alpha;
		switch (m_) {
			case 16:
				alpha = 0.673;
				break;
			case 32:
				alpha = 0.697;
				break;
			case 64:
				alpha = 0.709;
				break;
			default:
				alpha = 0.7213 / (1.0 + 1.079 / m_);
				break;
		}
		alphaMM_ = alpha * m_ * m_;
	}
	/**
	 * Adds element to the estimator
	 *
	 * @param[in] str string to add
	 * @param[in] len length of string
	 */
	void add(const char* str, uint32_t len) {
		uint64_t hash;
		MurmurHash3_x86_64(str, len, HLL_HASH_SEED, (void*) &hash);
		uint64_t index = hash >> (64 - b_);
		uint8_t rank = _GET_CLZ((hash << b_), 64 - b_);
		if (rank > M_[index]) {
			M_[index] = rank;
		}
	}
	/**
	 * Estimates cardinality value.
	 *
	 * @return Estimated cardinality value.
	 */
	double estimate(){
		double estimate;
		double sum = 0.0;
		for (uint32_t i = 0; i < m_; i++) {
			uint8_t rank = M_[i];
			if(rank>=63){
				rank=63; // in case of out of range
			}
			sum += 1.0 / (1UL << rank);
		}
		estimate = alphaMM_ / sum; // E in the original paper
		if (estimate <= 2.5 * m_) {
			uint32_t zeros = 0;
			for (uint32_t i = 0; i < m_; i++) {
				if (M_[i] == 0) {
					zeros++;
				}
			}
			if (zeros != 0) {
				estimate = m_ * std::log(static_cast<double>(m_)/ zeros);
			}
		}
		return estimate;
	}
	/**
	 * Merges the estimate from 'other' into this object, returning the estimate of their union.
	 * The number of registers in each must be the same.
	 *
	 * @param[in] other HyperLogLog instance to be merged
	 * 
	 * @exception std::invalid_argument number of registers doesn't match.
	 */
	void merge(const HyperLogLog& other) throw (std::invalid_argument) {
		if (m_ != other.m_) {
			std::stringstream ss;
			ss << "number of registers doesn't match: " << m_ << " != " << other.m_;
			throw std::invalid_argument(ss.str().c_str());
		}
		for (uint32_t r = 0; r < m_; ++r) {
			if (M_[r] < other.M_[r]) {
				M_[r] = other.M_[r];
			}
		}
	}
    // For Cython
	void update(std::vector<uint8_t>& newM){
		if(newM.size()!=m_){
			std::stringstream ss;
			ss << "number of registers doesn't match: " << m_ << " != " << newM.size();
			throw std::invalid_argument(ss.str().c_str());
		}
		for (uint32_t r = 0; r < m_; ++r){
			if (M_[r] < newM[r]) {
				M_[r] = newM[r];
			}
		}
	}
    // For Cython
	std::vector<uint8_t>& Array(){
		return M_;
	}
	/**
	 * Clears all internal registers.
	 */
	void clear() {
		std::fill(M_.begin(), M_.end(), 0);
	}

	/**
	 * Returns size of register.
	 *
	 * @return Register size
	 */
	uint32_t registerSize() const {
		return m_;
	}

	/**
	 * Exchanges the content of the instance
	 *
	 * @param[in,out] rhs Another HyperLogLog instance
	 */
	void swap(HyperLogLog& rhs) {
		std::swap(b_, rhs.b_);
		std::swap(m_, rhs.m_);
		std::swap(alphaMM_, rhs.alphaMM_);
		M_.swap(rhs.M_);	   
	}
	/**
	 * Dump the current status to a stream
	 *
	 * @param[out] os The output stream where the data is saved
	 *
	 * @exception std::runtime_error When failed to dump.
	 */
	void dump(std::ostream& os) const throw(std::runtime_error){
		os.write((char*)&b_, sizeof(b_));
		os.write((char*)&M_[0], sizeof(M_[0]) * M_.size());
		if(os.fail()){
			throw std::runtime_error("Failed to dump");
		}
	}
    /**
     * Count the sizeof Sketch
     *
     * @Return Size of Transmitting data
     */
	long sizeofSketch(){
		long size=0;
		size+=sizeof(b_);
		size+=sizeof(M_[0])*M_.size();
		return size;
	}
	/**
	 * Restore the status from a stream
	 * 
	 * @param[in] is The input stream where the status is saved
	 *
	 * @exception std::runtime_error When failed to restore.
	 */
	void restore(std::istream& is) throw(std::runtime_error){
		uint8_t b = 0;
		is.read((char*)&b, sizeof(b));
		HyperLogLog tempHLL(b);
		is.read((char*)&(tempHLL.M_[0]), sizeof(M_[0]) * tempHLL.m_);
		if(is.fail()){
		   throw std::runtime_error("Failed to restore");
		}	   
		swap(tempHLL);
	}
protected:
	uint8_t b_; ///< register bit width
	uint32_t m_; ///< register size
	double alphaMM_; ///< alpha * m^2
	std::vector<uint8_t> M_; ///< registers
};

class CountSketch {
public:
	/*
	 * Constuctor
	 * params are epsilon (between 0.01 and 1) and gamma (between 0.1 and 1)
	 */
	CountSketch(double epsilon=0.01, double gamma=0.1) {
		// calculate depth and width based on epsilon and gamma
		d = ceil(log(4 / gamma));
		w = ceil(1 / pow(epsilon, 2));

		// create Matrix
		srand(time(NULL)); // seed time to rand so we get some random numbers
		for (unsigned int i = 0; i < d; ++i) {
			std::vector<int> temp=std::vector<int>(w);
			C.push_back(temp); // create 'w' columns for each 'd' row
			seeds.push_back(rand()); // add random number to first seed function
			sign_seeds.push_back(rand()); // add random number to second seed function
		}
	}
	/*
	 * Add a string to the counter
	 */
	void addString(std::string s,int freq=1) {
		// use dafult C++ hasher to convert string to int
		int item = hasher(s);
		for (unsigned int i = 0; i < d; ++i) {
			// use value from seeds vector to seed the hashing function and create hash
			int p = murmurhash(&item, seeds[i]) % w;
			// use value from second seed vector (-1/+1)
			int sign = murmurhash(&item, sign_seeds[i]) % 2;
			// C = C + cg - update value
			C[i][p] += (sign * 2 - 1) * freq;
		}
	}
	/*
	 * Add an integer to the counter
	 */
	void addInt(int item,int freq=1) {
		for (unsigned int i = 0; i < d; ++i) {
			// use value from seeds vector to seed the hashing function and create hash
			int p = murmurhash(&item, seeds[i]) % w;
			// use value from second seed vector (-1/+1)
			int sign = murmurhash(&item, sign_seeds[i]) % 2;
			// C = C + cg - update value
			C[i][p] += (sign * 2 - 1) * freq;
		}
	}
	/*
	 * Get the frequency of a string
	 */
	double getStringFrequency(std::string s) {
		int item = hasher(s);
		double values[d];
		for (unsigned int i = 0; i < d; ++i) {
			int p = murmurhash(&item, seeds[i]) % w;
			int sign = murmurhash(&item, sign_seeds[i]) % 2;
			values[i] = (sign * 2 - 1) * C[i][p];
		}
		// return the median (4.3.2 The median trick, "ESTIMATING THE NUMBER OF DISTINCT ELEMENTS", page 18)
		std::nth_element(values, values + d / 2, values + d);
		return values[d / 2];
	}
	/*
	 * Get the frequency of an int
	 */
	double getIntFrequency(int item) {
		double values[d];
		for (unsigned int i = 0; i < d; ++i) {
			int p = murmurhash(&item, seeds[i]) % w;
			int sign = murmurhash(&item, sign_seeds[i]) % 2;
			values[i] = (sign * 2 - 1) * C[i][p];
		}
		// return the median (4.3.2 The median trick, "ESTIMATING THE NUMBER OF DISTINCT ELEMENTS", page 18)
		std::nth_element(values, values + d / 2, values + d);
		return values[d / 2];
	}
    /*
     * Estimate l2 norms
     */
	double estimatel2(){
		double values[d];
		for(unsigned int i=0;i<d;++i){
			for(unsigned int j=0;j<C[i].size();j++){
				values[i] += C[i][j]*C[i][j];
			}
		}
		std::nth_element(values, values + d / 2, values + d);
		return values[d / 2];
	}
    /*
     * Merge Count Sketch
     */
	void merge(CountSketch &cs){
		for(unsigned int i=0;i<d;++i){
			for(unsigned int j=0;j<C[i].size();j++){
				C[i][j] += cs.C[i][j];
			}
		}
	}
    // for Cython
	void update(std::vector<std::vector<int> >& _C){
		for(unsigned int i=0;i<d;++i){
			for(unsigned int j=0;j<C[i].size();j++){
				C[i][j] += _C[i][j];
			}
		}
	}
    // for Cython
	std::vector<std::vector<int>>& Array(){
		return C;
	}
    /**
     * Count the sizeof Sketch
     *
     * @Return Size of Transmitting data
     */
	long sizeofSketch(){
		long size=0;
		for(size_t i=0;i<d;++i){
			size+=sizeof(C[i][0])*C[i].size();
		}
		//size+=sign_seeds[0]*sign_seeds.size();
		return size;
	}
	/*
	 * Virtual destructor
	 */
	virtual ~CountSketch(){};
private:
	// depth and width of the matrix
	unsigned int d, w;
	// matrix
	std::vector<std::vector<int> > C;
	// vector of seeds
	std::vector<int> seeds;
	// 2nd vector of seeds (-1/+1)
	std::vector<int> sign_seeds;
	// string to int hasher
	std::hash<std::string> hasher;
};
} // namespace sketch

#endif // !defined(HYPERLOGLOG_HPP)
