// Copyright 2022 CodeBuddies ASE Group
//
// Crypto primitives wrappers and other helper functions
// for exchanges
//

#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <sstream>

#include "exchanges/helpers.h"

/*
 * Wrapper for the SHA256 hash function.
 */
std::vector<unsigned char> sha256_wrapper(std::string data) {
    std::vector<unsigned char> sha256_digest(SHA256_DIGEST_LENGTH);

    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, data.c_str(), data.length());
    SHA256_Final(sha256_digest.data(), &ctx);

    return sha256_digest;
}

/*
 * Wrapper for the HMAC function using SHA256 hashing algo.
 */
std::vector<unsigned char> hmac_sha512_wrapper(
    std::vector<unsigned char> data_vec,
    std::vector<unsigned char> key_vec) {
    unsigned int length = EVP_MAX_MD_SIZE;
    std::vector<unsigned char> hmac_digest(length);

    HMAC_CTX* ctx;
    ctx = HMAC_CTX_new();
    HMAC_Init_ex(ctx, key_vec.data(), key_vec.size(), EVP_sha512(), NULL);
    HMAC_Update(ctx, data_vec.data(), data_vec.size());
    HMAC_Final(ctx, hmac_digest.data(), &length);
    HMAC_CTX_free(ctx);

    return hmac_digest;
}

/*
 * Convert a vector of unsigned chars to string.
 */
std::string convert_vec_to_str(std::vector<unsigned char> data) {
    std::ostringstream output;

    for (unsigned char ch_x : data) {
        output << ch_x;
    }

    return output.str();
}

/*
 * Write callback function for cURL passed into request made to Kraken API.
 */
size_t kraken_write_callback(char* ptr, size_t size,
    size_t nmemb, void* userdata) {
    std::string* response = reinterpret_cast<std::string*> (userdata);
    response->append(ptr, size * nmemb);
    return size * nmemb;
}

/*
 * Convert a crow::rvalue object to a string
 */
std::string convert_to_string(const crow::json::rvalue jrvalue) {
    std::ostringstream os;
    os << jrvalue;
    std::string s = os.str();
    s = s.substr(1, s.length() - 2);
    return s;
}
