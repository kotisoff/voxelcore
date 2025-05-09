#pragma once

#include "typedefs.hpp"
#include "commons.hpp"
#include "ImageData.hpp"

class GBuffer : public Bindable {
public:
    GBuffer(uint width, uint height);
    ~GBuffer() override;

    void bind() override;
    void unbind() override;

    void bindBuffers() const;

    void resize(uint width, uint height);

    uint getWidth() const;
    uint getHeight() const;

    std::unique_ptr<ImageData> toImage() const;
private:
    uint width;
    uint height;

    uint fbo;
    uint colorBuffer;
    uint positionsBuffer;
    uint normalsBuffer;
    uint depthBuffer;

    void createColorBuffer();
    void createPositionsBuffer();
    void createNormalsBuffer();
    void createDepthBuffer();
};
