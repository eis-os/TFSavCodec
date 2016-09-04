/*
 * This file is part of tfsavcodec.
 * 
 * Copyright (c) 2016, Oskar Eisemuth
 * 
 * For the full copyright and license information,
 * please view the LICENSE file that was distributed with this source code.
 * 
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "lz4/lz4frame.h"
#include "memfunc.h"
#include "misc.h"

struct lz4helper_dctx {
	size_t srcSize;
	void* srcBuf;
	void* dstBuf;
	size_t dstSize;
	LZ4F_decompressionContext_t lz4ctx;
	const char* errstring;
};

typedef struct lz4helper_dctx lz4helper_dctx;

struct lz4helper_cctx {
	size_t srcSize;
	void* srcBuf;
	void* dstBuf;
	size_t dstSize;
	LZ4F_compressionContext_t lz4ctx;
	const char* errstring;
};

typedef struct lz4helper_cctx lz4helper_cctx;
#define MiB *(1 <<20)

bool decompressBufferInner(lz4helper_dctx* helper_ctx) {	
	LZ4F_decompressOptions_t decOpt;
	memset(&decOpt, 0, sizeof(decOpt));
	
	helper_ctx->dstSize = helper_ctx->srcSize;
	helper_ctx->dstBuf = memory_alloc(helper_ctx->dstSize);
	
	
	size_t srcPos = 0;
	size_t dstPos = 0;
	
	LZ4F_errorCode_t errOrSizeHint = 0;
	while(srcPos < helper_ctx->srcSize) {
		print(0, "Loop... \n");
		if ( (dstPos+(128 MiB)) >= helper_ctx->dstSize ) {
			helper_ctx->dstSize += (128 MiB);
			helper_ctx->dstBuf = memory_realloc(helper_ctx->dstBuf, helper_ctx->dstSize);
			print(0, "Reallocing DstBuffer %d\n", helper_ctx->dstSize);
		}
		
		const char* srcBufPtr = helper_ctx->srcBuf + srcPos;
		char* dstBufPtr = helper_ctx->dstBuf + dstPos;
		
	
		size_t srcSize = helper_ctx->srcSize - srcPos;
		size_t dstSize = helper_ctx->dstSize - dstPos;

		// decSize and srcSize are updated in LZ4F_decompress()
		errOrSizeHint = LZ4F_decompress(helper_ctx->lz4ctx, dstBufPtr, &dstSize, srcBufPtr, &srcSize, &decOpt);
		if(LZ4F_isError(errOrSizeHint)) {
			helper_ctx->errstring = LZ4F_getErrorName(errOrSizeHint);
			print_err(0, "%s\n", helper_ctx->errstring);
			return false;
		}

		dstPos += dstSize;
		srcPos += srcSize;
        }
	helper_ctx->dstSize = dstPos;
	return true;
}



bool decompressBuffer(void *inbuffer, size_t inlen, void **outbuffer, size_t* outlen) {
	lz4helper_dctx ctx;
	memset(&ctx, 0, sizeof(ctx));
	LZ4F_errorCode_t lz4err;
	
	ctx.srcBuf = inbuffer;
	ctx.srcSize = inlen;
	
	lz4err = LZ4F_createDecompressionContext(&(ctx.lz4ctx), LZ4F_VERSION);
	if(LZ4F_isError(lz4err)) {
		print_err(1, "LZ4 (createDecompressionContext) %s", LZ4F_getErrorName(lz4err));
		exit(-1);
	}
	
	
	if (!decompressBufferInner(&ctx)) {
		print_err(1, "LZ4 %s\n", ctx.errstring);
		exit(-1);
	}
	
	lz4err = LZ4F_freeDecompressionContext(ctx.lz4ctx);
	if(LZ4F_isError(lz4err)) {
		print_err(1, "LZ4 (freeDecompressionContext) %s\n", LZ4F_getErrorName(lz4err));
		exit(-1);
	}
	
	*outbuffer = ctx.dstBuf;
	*outlen = ctx.dstSize;
	return true;
}

bool compressBufferInner(lz4helper_cctx* helper_ctx) {	
	LZ4F_preferences_t compressPref;
	memset(&compressPref, 0, sizeof(compressPref));
	compressPref.compressionLevel = 0;
	compressPref.frameInfo.blockSizeID = LZ4F_max256KB;
	compressPref.frameInfo.blockMode = LZ4F_blockIndependent; // LZ4F_blockLinked; TF braucht LZ4F_blockIndependent
	compressPref.frameInfo.contentChecksumFlag = LZ4F_contentChecksumEnabled;
	compressPref.frameInfo.frameType = LZ4F_frame;
	compressPref.frameInfo.blockChecksumFlag = LZ4F_blockChecksumEnabled; // LZ4F_noBlockChecksum;
	
	compressPref.frameInfo.contentSize = 0; //helper_ctx->srcSize;  // TF braucht 0
	
	helper_ctx->dstSize = helper_ctx->srcSize;
	helper_ctx->dstBuf = memory_alloc(helper_ctx->dstSize);
	
	
	size_t srcPos = 0;
	size_t dstPos = 0;
	
	size_t srcSize = 0;
	size_t dstSize = 0;
	
	srcSize = helper_ctx->srcSize - srcPos;
	dstSize = helper_ctx->dstSize - dstPos;
	
	LZ4F_errorCode_t errOrSizeHint = 0;
	
	errOrSizeHint = LZ4F_compressBegin(helper_ctx->lz4ctx, helper_ctx->dstBuf + dstPos, dstSize, &compressPref);
	
	if(LZ4F_isError(errOrSizeHint)) {
		helper_ctx->errstring = LZ4F_getErrorName(errOrSizeHint);
		print_err(0, "%s\n", helper_ctx->errstring);
		return false;
	}
	dstPos += errOrSizeHint;
		
	size_t maxsize = LZ4F_compressBound(helper_ctx->srcSize, &compressPref);
	if ( (dstPos+(maxsize)) >= helper_ctx->dstSize ) {
		helper_ctx->dstSize += maxsize;
		helper_ctx->dstBuf = memory_realloc(helper_ctx->dstBuf, helper_ctx->dstSize);
		print(0, "Reallocing DstBuffer %d (added %d)\n", helper_ctx->dstSize, maxsize);
	}
	
	const char* srcBufPtr = helper_ctx->srcBuf + srcPos;
	char* dstBufPtr = helper_ctx->dstBuf + dstPos;
		
	
	srcSize = helper_ctx->srcSize - srcPos;
	dstSize = helper_ctx->dstSize - dstPos;


	errOrSizeHint = LZ4F_compressUpdate(helper_ctx->lz4ctx, dstBufPtr, dstSize, srcBufPtr, srcSize, NULL);
	if(LZ4F_isError(errOrSizeHint)) {
		helper_ctx->errstring = LZ4F_getErrorName(errOrSizeHint);
		print_err(0, "%s\n", helper_ctx->errstring);
		return false;
	}

	dstPos += errOrSizeHint;
	srcPos += srcSize;
	
	// Make room for remaining data
	if ( (dstPos+(128 MiB)) >= helper_ctx->dstSize ) {
		helper_ctx->dstSize += (128 MiB);
		helper_ctx->dstBuf = memory_realloc(helper_ctx->dstBuf, helper_ctx->dstSize);
		print(0, "Reallocing DstBuffer for compressEnd %d\n", helper_ctx->dstSize);
	}
	
	errOrSizeHint = LZ4F_compressEnd(helper_ctx->lz4ctx, helper_ctx->dstBuf + dstPos, helper_ctx->dstSize - dstPos, NULL);
	if(LZ4F_isError(errOrSizeHint)) {
		helper_ctx->errstring = LZ4F_getErrorName(errOrSizeHint);
		print_err(0, "%s\n", helper_ctx->errstring);
		return false;
	}
	dstPos += errOrSizeHint;
	
	helper_ctx->dstSize = dstPos;
	return true;
}



bool compressBuffer(void *inbuffer, size_t inlen, void **outbuffer, size_t* outlen) {
	lz4helper_cctx ctx;
	memset(&ctx, 0, sizeof(ctx));
	LZ4F_errorCode_t lz4err;
	
	ctx.srcBuf = inbuffer;
	ctx.srcSize = inlen;
	
	lz4err = LZ4F_createCompressionContext(&(ctx.lz4ctx), LZ4F_VERSION);
	if(LZ4F_isError(lz4err)) {
		print_err(1, "LZ4 (createCompressionContext) %s", LZ4F_getErrorName(lz4err));
		exit(-1);
	}
	
	
	if (!compressBufferInner(&ctx)) {
		print_err(1, "LZ4 %s\n", ctx.errstring);
		exit(-1);
	}
	
	lz4err = LZ4F_freeCompressionContext(ctx.lz4ctx);
	if(LZ4F_isError(lz4err)) {
		print_err(1, "LZ4 (freeCompressionContext) %s\n", LZ4F_getErrorName(lz4err));
		exit(-1);
	}
	
	*outbuffer = ctx.dstBuf;
	*outlen = ctx.dstSize;
	return true;
}
