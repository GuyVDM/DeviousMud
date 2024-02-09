#include "Shared/Utilities/vec3.hpp"

class Camera 
{
public:
	void set_position(const Utilities::ivec3& _pos);

	const Utilities::ivec3& get_position() const;

	void set_render_distance(const uint32_t& _rd);

	const uint32_t& get_render_distance() const;

public:
	Camera();
	Camera(uint32_t _rd);
	Camera(Utilities::ivec3 _pos, uint32_t _rd);
	~Camera() = default;

private:
	Utilities::ivec3 pos;
	uint32_t         render_distance;

};
