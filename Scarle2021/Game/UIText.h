#pragma once
#include "ButtonInterface.h"

class UIText : public ButtonInterface
{
public:
	UIText(const std::string _string, const Vector2& bt_pos, ID3D11Device* _d3dDevice);
	~UIText() override;

	//Scarle 
	void update(GameData* _GD, const Vector2& mouse_pos) override;
	void render(DrawData2D* _DD2D) override;

	//Getters & Setters
	void setText(const std::string _string);
	void setBgColor(float* color);
	void setBgSize(const Vector2& new_size) const;
	void setTextColor(float* color);
	void setTextSize(const Vector2& new_size) const;
	void setPos(const Vector2& new_pos) override;
	const Vector2& getPos() override;
	const Vector2& getRes() override;

private:
	//Saved colors 
	float* current_bg_color = nullptr;
	float* current_text_color = nullptr;
};
