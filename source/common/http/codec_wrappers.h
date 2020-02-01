#pragma once

#include "envoy/http/codec.h"

namespace Envoy {
namespace Http {

/**
 * Wrapper for ResponseStreamDecoder that just forwards to an "inner" decoder.
 */
class ResponseStreamDecoderWrapper : public ResponseStreamDecoder {
public:
  // StreamDecoder
  void decode100ContinueHeaders(HeaderMapPtr&& headers) override {
    inner_.decode100ContinueHeaders(std::move(headers));
  }

  void decodeHeaders(HeaderMapPtr&& headers, bool end_stream) override {
    if (end_stream) {
      onPreDecodeComplete();
    }

    inner_.decodeHeaders(std::move(headers), end_stream);

    if (end_stream) {
      onDecodeComplete();
    }
  }

  void decodeData(Buffer::Instance& data, bool end_stream) override {
    if (end_stream) {
      onPreDecodeComplete();
    }

    inner_.decodeData(data, end_stream);

    if (end_stream) {
      onDecodeComplete();
    }
  }

  void decodeTrailers(HeaderMapPtr&& trailers) override {
    onPreDecodeComplete();
    inner_.decodeTrailers(std::move(trailers));
    onDecodeComplete();
  }

  void decodeMetadata(MetadataMapPtr&& metadata_map) override {
    inner_.decodeMetadata(std::move(metadata_map));
  }

protected:
  ResponseStreamDecoderWrapper(ResponseStreamDecoder& inner) : inner_(inner) {}

  /**
   * Consumers of the wrapper generally want to know when a decode is complete. This is called
   * at that time and is implemented by derived classes.
   */
  virtual void onPreDecodeComplete() PURE;
  virtual void onDecodeComplete() PURE;

  ResponseStreamDecoder& inner_;
};

/**
 * Wrapper for RequestStreamEncoder that just forwards to an "inner" encoder.
 */
class RequestStreamEncoderWrapper : public RequestStreamEncoder {
public:
  // StreamEncoder
  void encode100ContinueHeaders(const HeaderMap& headers) override {
    inner_.encode100ContinueHeaders(headers);
  }

  void encodeHeaders(const HeaderMap& headers, bool end_stream) override {
    inner_.encodeHeaders(headers, end_stream);
    if (end_stream) {
      onEncodeComplete();
    }
  }

  void encodeData(Buffer::Instance& data, bool end_stream) override {
    inner_.encodeData(data, end_stream);
    if (end_stream) {
      onEncodeComplete();
    }
  }

  void encodeTrailers(const HeaderMap& trailers) override {
    inner_.encodeTrailers(trailers);
    onEncodeComplete();
  }

  void encodeMetadata(const MetadataMapVector& metadata_map_vector) override {
    inner_.encodeMetadata(metadata_map_vector);
  }

  Stream& getStream() override { return inner_.getStream(); }

protected:
  RequestStreamEncoderWrapper(RequestStreamEncoder& inner) : inner_(inner) {}

  /**
   * Consumers of the wrapper generally want to know when an encode is complete. This is called at
   * that time and is implemented by derived classes.
   */
  virtual void onEncodeComplete() PURE;

  RequestStreamEncoder& inner_;
};

} // namespace Http
} // namespace Envoy
