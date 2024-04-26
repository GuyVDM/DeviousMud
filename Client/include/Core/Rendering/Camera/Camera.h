#include "Shared/Utilities/vec2.hpp"

class Camera 
{
public:
	void set_position(const Utilities::vec2& _pos);

	const Utilities::vec2& get_position() const;

public:
	Camera();
	Camera(Utilities::vec2 _pos);
	~Camera() = default;

private:
	Utilities::vec2  m_pos;

};
