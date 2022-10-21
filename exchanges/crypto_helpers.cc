#include "../common/crypto_helpers.h"
#include <openssl/hmac.h>
#include <openssl/sha.h>

std::vector<unsigned char> sha256_wrapper(std::string data) { 
    std::vector<unsigned char> sha256_digest (SHA256_DIGEST_LENGTH);

    if (data.empty()) {
        goto err_out;
    }

    SHA256_CTX ctx;
    
    if (SHA256_Init(&ctx) == 0) {
        goto err_out;
    }

    if (SHA256_Update(&ctx, data.c_str(), data.length()) == 0) {
        goto err_out;
    }
    
    if (SHA256_Final(sha256_digest.data(), &ctx) == 0) {
        goto err_out;
    }

    return sha256_digest;

err_out:
    return std::vector<unsigned char>();
}

std::vector<unsigned char> hmac_sha512_wrapper(std::string data, std::string key) {
    std::vector<unsigned char> hmac_digest (EVP_MAX_MD_SIZE);
    if (data.empty() || key.empty()) {
        goto err_out;
    }

    std::vector<unsigned char> data_vec;
    std::vector<unsigned char> key_vec;

    for (unsigned char x_ch : data) {
        data_vec.push_back(x_ch);
    }

    for (unsigned char x_ch : key) {
        key_vec.push_back(x_ch);
    }

    HMAC_CTX *ctx;
    if ((ctx = HMAC_CTX_new()) == NULL) {
        goto err_out;
    }

    if (HMAC_CTX_Init_ex(ctx, key_vec.data(), key_vec.size(), EVP_sha512(), NULL) == 0) {
        goto err_out;
    }

    if (HMAC_Update(ctx, data_vec.data(), data_vec.size()) == 0) {
        goto err_out;
    }


    if (HMAC_Final(ctx, hmac_digest.data(), hmac_digest.size()) == 0) {
        goto err_out;
    }

    HMAC_CTX_free(ctx);

    return hmac_digest;

err_out:
    return std::vector<unsigned int>();
}

std::string convert_vec_to_str(std::vector<unsigned char> data) {
    std::ostringstream output;

    for (unsigned char ch_x : data) {
        output << ch_x;
    }

    return output.str();
}
