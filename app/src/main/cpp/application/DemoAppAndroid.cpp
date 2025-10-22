#include "application/DemoAppAndroid.h"
#include "scenes/menu_scene/MenuScene.h"

DemoAppAndroid::DemoAppAndroid(android_app* app)
    : AppAndroid(app) {}

DemoAppAndroid::~DemoAppAndroid() {}

void DemoAppAndroid::loadResources() {
    clay::Resources::Handle<vk::Sampler> samplerHandle_Default;

    clay::Resources::Handle<clay::PipelineResource> pipelineHandle_TextureDepth;

    clay::Resources::Handle<clay::Texture> textureHandle_VTexture;
    clay::Resources::Handle<clay::Texture> textureHandle_Solid;

    clay::Resources::Handle<clay::Material> materialHandle_Solid;
    clay::Resources::Handle<clay::Material> materialHandle_VTexture;

    clay::Resources::Handle<clay::Mesh> meshHandle_Sphere;

    // shaders
    clay::ShaderModule textureVertShader(
        mpGraphicsContext_->getDevice(),
        vk::ShaderStageFlagBits::eVertex,
        loadFileToMemory("shaders/Texture.vert.spv")
    );
    clay::ShaderModule textureFragShader(
        mpGraphicsContext_->getDevice(),
        vk::ShaderStageFlagBits::eFragment,
        loadFileToMemory("shaders/Texture.frag.spv")
    );

    // Font
    {
        // load font
        auto fontData = loadFileToMemory(
            (clay::Resources::getResourcePath() / "fonts/runescape_uf.ttf").string()
        );
        clay::ShaderModule fontVertShader(
            mpGraphicsContext_->getDevice(),
            vk::ShaderStageFlagBits::eVertex,
            loadFileToMemory("shaders/Text.vert.spv")
        );
        clay::ShaderModule fontFragShader(
            mpGraphicsContext_->getDevice(),
            vk::ShaderStageFlagBits::eFragment,
            loadFileToMemory("shaders/Text.frag.spv")
        );

        mResources_.addResource<clay::Font>(
            clay::Font(
                *mpGraphicsContext_,
                fontData,
                fontVertShader,
                fontFragShader,
                *((clay::GraphicsContextAndroid*)mpGraphicsContext_.get())->mCameraUniform_
            ),
            "Runescape"
        );
    }

    // Sampler
    {
        // Default sampler
        vk::Sampler sampler;

        vk::SamplerCreateInfo samplerInfo{};
        samplerInfo.magFilter = vk::Filter::eNearest;
        samplerInfo.minFilter = vk::Filter::eNearest;
        samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToBorder;
        samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToBorder;
        samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToBorder;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 1.0f;
        samplerInfo.borderColor = vk::BorderColor::eFloatTransparentBlack;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = vk::CompareOp::eAlways;
        samplerInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;
        samplerInfo.mipLodBias = 0.0f;

        vk::Result result = mpGraphicsContext_->getDevice().createSampler(&samplerInfo, nullptr, &sampler);
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("failed to create texture sampler!");
        }

        samplerHandle_Default = mResources_.addResource<vk::Sampler>(std::move(sampler), "Default");
    }
    {
        // sphere mesh
        meshHandle_Sphere = mResources_.loadResource<clay::Mesh>(
            {(clay::Resources::getResourcePath() / "models/Sphere.obj").string()},
            "Sphere"
        );
    }
    // images
    {
        clay::utils::ImageData imageData = loadImageFileToMemory(clay::Resources::getResourcePath() / "textures/V.png");

        clay::Texture vTexture(*mpGraphicsContext_);
        vTexture.initialize(imageData);
        vTexture.setSampler(mResources_[samplerHandle_Default]);

        textureHandle_VTexture = mResources_.addResource(std::move(vTexture), "VTexture");
    }
    {
        // solid image
        clay::utils::ImageData singleRGBA;
        singleRGBA.width = 1;
        singleRGBA.height = 1;
        singleRGBA.channels = 4; // RGBA
        singleRGBA.pixels = std::make_unique<uint8_t[]>(4);

        singleRGBA.pixels[0] = 255;
        singleRGBA.pixels[1] = 255;
        singleRGBA.pixels[2] = 255;
        singleRGBA.pixels[3] = 255;

        clay::Texture solidTexture(*mpGraphicsContext_);
        solidTexture.initialize(singleRGBA);
        solidTexture.setSampler(mResources_[samplerHandle_Default]);

        textureHandle_Solid = mResources_.addResource(std::move(solidTexture), "SolidTexture");
    }

    // pipeline
    {
        // (TextureDepth)
        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.pipelineLayoutInfo.shaders = {
            &textureVertShader, &textureFragShader
        };

        auto vertexAttrib = clay::Mesh::Vertex::getAttributeDescriptions();
        pipelineConfig.pipelineLayoutInfo.attributeDescriptions = {vertexAttrib.begin(), vertexAttrib.end()};
        pipelineConfig.pipelineLayoutInfo.vertexInputBindingDescription = clay::Mesh::Vertex::getBindingDescription();

        pipelineConfig.pipelineLayoutInfo.depthStencilState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
        };

        pipelineConfig.pipelineLayoutInfo.rasterizerState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1.0f,
        };

        pipelineConfig.pipelineLayoutInfo.pushConstants = {
            {
                .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                .offset = 0,
                .size = sizeof(glm::mat4) + sizeof(glm::vec4)
            }
        };

        pipelineConfig.bindingLayoutInfo.bindings = {
            {
                .binding = 0,
                .descriptorType =  vk::DescriptorType::eUniformBuffer,
                .descriptorCount = 1,
                .stageFlags = vk::ShaderStageFlagBits::eVertex,
                .pImmutableSamplers = nullptr
            },
            {
                .binding = 1,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                .descriptorCount = 1,
                .stageFlags = vk::ShaderStageFlagBits::eFragment,
                .pImmutableSamplers = nullptr
            }
        };

        pipelineHandle_TextureDepth = mResources_.addResource<clay::PipelineResource>(
            clay::PipelineResource(pipelineConfig),
            "TextureDepth"
        );

    }
    // Material
    {
        // Single White
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_TextureDepth]
        };

        matConfig.bufferBindings = {
            {
                .buffer = ((clay::GraphicsContextAndroid*)mpGraphicsContext_.get())->mCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = vk::DescriptorType::eUniformBuffer
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_Solid].getSampler(),
                .imageView = mResources_[textureHandle_Solid].getImageView(),
                .binding = 1,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler
            }
        };


        materialHandle_Solid = mResources_.addResource<clay::Material>(
            clay::Material(matConfig),
            "SolidTexture"
        );
    }
    {
        // VTexture
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_TextureDepth]
        };

        matConfig.bufferBindings = {
            {
                .buffer = ((clay::GraphicsContextAndroid*)mpGraphicsContext_.get())->mCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = vk::DescriptorType::eUniformBuffer
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_VTexture].getSampler(),
                .imageView = mResources_[textureHandle_VTexture].getImageView(),
                .binding = 1,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler
            }
        };

        materialHandle_VTexture = mResources_.addResource<clay::Material>(
            clay::Material(matConfig),
            "VTexture"
        );
    }
    // Models
    {
        // Solid Sphere
        clay::Model model(*mpGraphicsContext_);
        model.addElement({
            &mResources_[meshHandle_Sphere],
            &mResources_[materialHandle_Solid],
            glm::mat4(1),
        });

        mResources_.addResource<clay::Model>(std::move(model), "SolidSphere");
    }
    {
        // V Texture Sphere
        clay::Model model(*mpGraphicsContext_);
        model.addElement({
            &mResources_[meshHandle_Sphere],
            &mResources_[materialHandle_VTexture],
            glm::mat4(1),
        });

        mResources_.addResource<clay::Model>(std::move(model), "VTexture");
    }
}
