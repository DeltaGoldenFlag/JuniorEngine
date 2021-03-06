#pragma once
/*
 * Author: David Wong
 * Email: david.wongcascante@digipen.edu
 * File Name: Sprite.h
 * Description: Sprites manage the raw data that comes from textures and updates the texture coordinates for texture maps
 * Created: 13 Feb 2019
 * Last Modified: 13 May 2019
*/

// Includes
#include "Component.h"				// Component
#include <string>					// Strings
#include <Vec3.h>					// Vec3

namespace Junior
{
	// Forward Declarations
	class MemoryManager;
	class Texture;
	class TextureAtlas;
	struct AtlasNode;

	class Sprite : public Component<Sprite>
	{
	private:
		// Private Member Variables

		// Whether we already have a spot on the texture atlas
		bool isOnAtlas_;
		// The texture atlas offset
		Vec3 atlasOffset_;
		// The texture atlas scale
		Vec3 atlasScale_;
		// The path to the texture
		std::string path_;
		// The sprite's texture
		Texture* texture_;
		// The node of the texture atlas we are using
		AtlasNode* atlasNode_;
		// The texture atlas we are using
		TextureAtlas* atlas_;

	public:
		// Public Member Functions

		// Constructs an empty sprite component
		Sprite();
		// Constructs a sprite component with texture data from a file
		// Params:
		//	path: The path to the imag file
		Sprite(const std::string& path);
		// Copy Constructor
		// Params:
		//	other: The other sprite we are trying to copy from
		Sprite(const Sprite& other);
		// Initializes the component
		void Initialize() override;
		// Updates the component
		// Params:
		//	dt: The time in between frames
		void Update(double dt) override;
		// Cleans up the component
		// Params:
		//	The memory manager used to create this object
		void Unload() override;
		// Modifies the sprite's UV modification
		// Params:
		//	xOffset: The x offset
		//	yOffset: The y offset
		//	xScale: The xscale
		//	yScale: The y scale
		void SetUVModifications(float xOffset, float yOffset, float xScale, float yScale);
		// Modfies the sprite's UV modification
		// Params:
		//	xOffset: The x offset
		//	yOffset: The y offset
		void SetUVModifications(float xOffset, float yOffset);
		// Loads an image to the sprite's texture
		// Params:
		//	path: The path to the image file
		void LoadFromDisk(const std::string& path);
		// Gets the immediate texture from this sprite
		// Returns: The sprite's texture
		Texture* GetTexture() const;
		// Returns: The atlas node used to store this sprite's subtexture
		AtlasNode* GetNode() const;
		// Gets the atlas offset from Sprite
		// Returns: The sprite's offset from the atlas
		const Vec3& GetAtlasOffset() const;
		// Gets the atlas scale from Sprite
		// Returns: The sprite's scale from the atlas
		const Vec3& GetAtlasScale() const;
		// Gets the atlas' id
		unsigned GetAtlasID() const;
		// Writes the component to a file
		// Params:
		//	parser: The parser used to write the component
		// Throws: ParserException
		void Serialize(Parser& parser) const override;
		// Reads and creates an object from a file
		// Params:
		//	parser: The parser used to read the component from the file
		// Throws: ParserException
		void Deserialize(Parser& parser) override;
	};
}