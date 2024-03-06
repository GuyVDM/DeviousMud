#include "Shared/Utilities/vec2.hpp"

class Camera 
{
public:
	void set_position(const Utilities::vec2& _pos);

	const Utilities::vec2& get_position() const;

	void set_render_distance(const uint32_t& _rd);

	const uint32_t& get_render_distance() const;

public:
	Camera();
	Camera(uint32_t _rd);
	Camera(Utilities::vec2 _pos, uint32_t _rd);
	~Camera() = default;

private:
	Utilities::vec2  pos;
	uint32_t         render_distance;

};
