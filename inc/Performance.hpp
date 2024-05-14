/*!
 \file Performance.hpp
 \brief Class to store performance of CPU
 \author Màrius Montón
 \date Aug 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include <systemc>
#include <tlm>

/**
 * @brief Performance indicators class
 *
 * Singleton class to be shared among all other classes
 */
class Performance {
public:

	/**
	 * @brief Get an instance of the class
	 * @return pointer to Performance class
	 */
	static Performance* getInstance();

	/**
	 * @brief Increment data memory read counter
	 */
	inline void dataMemoryRead() {
		data_memory_read++;
	}

	/**
	 * @brief Increment data memory write counter
	 */
	inline void dataMemoryWrite() {
		data_memory_write++;
	}

	/**
	 * @brief Increment code memory read counter
	 */
	inline void codeMemoryRead() {
		code_memory_read++;
	}

	/**
	 * @brief Increment code memory write counter
	 */
	inline void codeMemoryWrite() {
		code_memory_write++;
	}

	/**
	 * @brief Increment register read counter
	 */
	inline void registerRead() {
		register_read++;
	}

	/**
	 * @brief Increment register write counter
	 */
	inline void registerWrite() {
		register_write++;
	}

	/**
	 * @brief Increment instructions executed counter
	 */
	inline void instructionsInc() {
		instructions_executed++;
	}

	/**
	 * @brief Dump counters to cout
	 */
	void dump() const;

	inline uint_fast64_t getInstructions() const {
	  return instructions_executed;
	}

private:
	static Performance *instance;
	Performance();

	uint_fast64_t data_memory_read;
	uint_fast64_t data_memory_write;
	uint_fast64_t code_memory_read;
	uint_fast64_t code_memory_write;
	uint_fast64_t register_read;
	uint_fast64_t register_write;
	uint_fast64_t instructions_executed;
};

#endif
