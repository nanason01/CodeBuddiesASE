#pragma once

#include "common/types.h"

#include <vector>
#include <string>
#include <crow.h>

/*
 * Helper function for sha256 digest.
 */
std::vector<unsigned char> sha256_wrapper(std::string data);

/*
 * Helper function for hmac with sha-512.
 */
std::vector<unsigned char> hmac_sha512_wrapper(std::vector<unsigned char> data, std::vector<unsigned char> secret_key);

/*
 * Helper function for hmac with sha-256.
 */
std::vector<unsigned char> hmac_sha256_wrapper(std::vector<unsigned char> data, std::vector<unsigned char> secret_key);

/*
 * Helper function for converting a vector of unsigned char's to string
 * using ostringstream.
 */
std::string convert_vec_to_str(std::vector<unsigned char> data);

/*
 * Write callback function for cURL.
 */
size_t kraken_write_callback(char *ptr, size_t size, size_t nmeb, void *userdata);

/*
 * Helper to convert crow::json to string.
 */
std::string convert_to_string(const crow::json::rvalue jrvalue);