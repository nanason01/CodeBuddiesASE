#include "../common/crypto_helpers.h"
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <sstream>
std::vector<unsigned char> sha256_wrapper(std::string data) { 
    std::vector<unsigned char> sha256_digest (SHA256_DIGEST_LENGTH);
    if (data.empty()) {
        goto err_out;
    }

    SHA256_CTX ctx; 
    if (!SHA256_Init(&ctx)) {
        goto err_out;
    }

    if (!SHA256_Update(&ctx, data.c_str(), data.length())) {
        goto err_out;
    }
    
    if (!SHA256_Final(sha256_digest.data(), &ctx)) {
        goto err_out;
    }

    return sha256_digest;

err_out:
    return std::vector<unsigned char>();
}

std::vector<unsigned char> hmac_sha512_wrapper(std::string data, API_key key) {
    std::vector<unsigned char> data_vec;
    std::vector<unsigned char> key_vec;

    unsigned int length = EVP_MAX_MD_SIZE;
    std::vector<unsigned char> hmac_digest (length);

    if (data.empty() || key.empty()) {
        goto err_out;
    }

    for (unsigned char x_ch : data) {
        data_vec.push_back(x_ch);
    }

    for (unsigned char x_ch : key) {
        key_vec.push_back(x_ch);
    }

    HMAC_CTX *ctx;
    if (!(ctx = HMAC_CTX_new())) {
        goto err_out;
    }

    if (!HMAC_Init_ex(ctx, key_vec.data(), key_vec.size(), EVP_sha512(), NULL)) {
        goto err_out;
    }

    if (!HMAC_Update(ctx, data_vec.data(), data_vec.size())) {
        goto err_out;
    }

    if (!HMAC_Final(ctx, hmac_digest.data(), &length)) {
        goto err_out;
    }

    HMAC_CTX_free(ctx);
    return hmac_digest;

err_out:
    return std::vector<unsigned char>();
}

std::string convert_vec_to_str(std::vector<unsigned char> data) {
    std::ostringstream output;

    for (unsigned char ch_x : data) {
        output << ch_x;
    }

    return output.str();
}
