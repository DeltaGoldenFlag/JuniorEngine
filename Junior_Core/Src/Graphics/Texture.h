#pragma once
/*
* Author: David Wong
* Email: david.wongcascante@digipen.edu
* File name: Texture.h
* Description: Describe and write the functionality of the Texture data used by the Junior game Engine
* Created: 5-Aug-2018
* Last Modified: 19-Jan-2019
*/

// Includes //
#include <string>

#include "Resource.h"

namespace Junior
{
	class Texture : public Resource
	{
	private:
		// Private Class Variables //
		// The texture's id inside of OpenGL
		unsigned textureID_ = 0;
		// Whether the texture is 1D, 2D, or 3D
		unsigned typeOfTexture_ = 0;
		// How many textures we are storing if this texture is supposed to be a texture array
		unsigned arrayCount_ = 0;
		// Whether the texture is RGB, RGBA, BRG, etc. (as an OpenGL enum)
		unsigned formatOfTexture_ = 0;
		// Index 0 is the width of the texture, index 1 is the height of the texture, index 2 is for the depth of the image (3D textures)
		unsigned dimensions_[3] = { 0, 0, 0 };
		// The pixels of the image
		unsigned char* pixels_ = nullptr;
	public:
		// Public Member Functions ///
		// Creates an empty texture with the size specified
		// Params:
		//	textureFormat: The texture's format
		//	textureType: 1D, 2D, or 3D texture
		//	generateMipMaps: Are we generating mipmaps for this texture
		//	textureWidth: width
		//	textureHeight: height
		//	textureDepth: Usually 1 if it is not a 3D texture, otherwise it is how big the texture is on the 3rd dimension
		Texture(unsigned int textureFormat, unsigned int textureType, bool generateMipMaps, int textureWidth, int textureHeight, int textureDepth);

		// Creates a empty texture
		Texture();

		// Creates a texture from a file on disk
		// Params:
		//	resouceDir: The location the resource is located
		void LoadFromDisk(std::string resourceDir) override;
		// Cleans up the texture
		void CleanUp() override;

		// Returns: The ID OpenGL gave to this texture
		const unsigned& GetOpenGLTextureID() const;
		// Bind the texture to the current shader
		void BindTexture() const;
		// Adds another image from a file to this texture array
		// Params:
		//	resourceDir: The directory to the file we want to load
		void AppendedLoadToTextureArray2D(std::string resourceDir);
		// Unbinds the texture from the current shader
		void UnbindTexture() const;
		// Returns: the pixels made by OpenGL of the texture
		const unsigned char* GetPixels();

		// Static Helper Functions
		// Gets thep pixels of an image file on the disk
		// Params:
		//	resourceDir: The location the resource is located
		//	width: Returns the width of the image
		//	height: Returns the height of the image
		//	channels: Returns the channels of the image
		// Returns: The pixels of said image file
		static unsigned char* GetPixelsFromFile(std::string resourceDir, int* width = nullptr, int* height = nullptr, int* channels = nullptr);
	};
}