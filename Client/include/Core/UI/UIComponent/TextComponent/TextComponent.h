#pragma once
#include "Core/UI/UIComponent/UIComponent.h"

#include "Core/Rendering/Fonts/FontsConfig.h"


struct TextArgs
{
	SDL_Color  color;
	int        size;
	e_FontType font;
	bool	   bDropShadow;

	TextArgs() 
	{
		color = { 255 ,255, 255, 255 };
		size = 10;
		font = e_FontType::RUNESCAPE_UF;
		bDropShadow = false;
	}

	const static TextArgs Default()
	{
		TextArgs optionArgs{};
		optionArgs.color = { 255, 255, 255, 255 };
		optionArgs.size = 10;
		optionArgs.font = e_FontType::RUNESCAPE_UF;
		optionArgs.bDropShadow = false;
		return optionArgs;
	}
};

struct SDL_Texture;
namespace Graphics
{
	class TextComponent final : public UIComponent
	{
	public:
		static std::shared_ptr<TextComponent> create_text(std::string _contents, Utilities::vec2 _pos, const TextArgs _args = TextArgs::Default());

		virtual ~TextComponent();

	protected:
		virtual void init() override;

	private:
		using UIComponent::UIComponent;

		SDL_Texture* textTexture;
		TextArgs     optionArgs;

	    void renderText(std::shared_ptr<Graphics::Renderer> _renderer);

		friend class TextComponent;
		friend class std::shared_ptr<TextComponent>;
	};
}