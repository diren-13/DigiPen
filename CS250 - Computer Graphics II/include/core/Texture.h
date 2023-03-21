/*+======================================================================================
File:       Texture.h

Date:       7th April 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Interface for a Texture.

Classes:    RTR::Texture
======================================================================================+*/

#pragma once

// Standard Libraries
#include <string>
// External Dependencies
#include <vulkan/vulkan.hpp>

namespace RTR
{

    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /// <summary>
    /// Encapsulates a Texture object
    /// </summary>
    class Texture
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        enum class Type
        {
            Linear,
            sRGB
        };


        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        Texture (Device& device);
        Texture (Device& device, const std::string& filepath, Type _type = Type::Linear);
        Texture (Device& device, const std::filesystem::path& filepath, Type _type = Type::Linear);
        Texture (Texture&& rhs);
        Texture& operator=(Texture&& rhs);
        ~Texture();

        Texture (const Texture& rhs)            = delete;
        Texture& operator=(const Texture& rhs)  = delete;

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]]   vk::Image       GetHandle()     const   { return handle; }
        [[nodiscard]]   vk::ImageView   GetImageView()  const   { return imageView; }
        [[nodiscard]]   uint32_t        GetWidth()      const   { return width; }
        [[nodiscard]]   uint32_t        GetHeight()     const   { return height; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        bool Load   (const std::string& filepath, Type _type);
        bool Load   (const std::filesystem::path& filepath, Type _type);

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Device*             device;

        vk::Image           handle;
        vk::ImageView       imageView;
        vk::DeviceMemory    imageMemory;

        bool                loaded;
        Type                type;
        uint32_t            width;
        uint32_t            height;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void transitionLayout   (vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
        void copyBufferToImage  (vk::Buffer buffer);
        void createImageView    (vk::Format format);
    };

} // namespace RTR

