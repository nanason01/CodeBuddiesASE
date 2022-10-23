#pragma once

#include <vector>
#include <string>
#include "types.h"

/*
 * Helper function for sha256 digest.
 */
std::vector<unsigned char> sha256_wrapper(std::string data);

/*
 * Helper function for hmac with sha-512.
 */
std::vector<unsigned char> hmac_sha512_wrapper(std::string data, API_key secret_key);

/*
 * Helper function for converting a vector of unsigned char's to string
 * using ostringstream.
 */
std::string convert_vec_to_str(std::vector<unsigned char> data);
