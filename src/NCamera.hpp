#ifndef NAELSTROF_CAMERA
#define NAELSTROF_CAMERA

class NCamera : public NNode
{
public:
	NCamera();
	float GetZoom();
	void SetZoom(float); //Zoom
	glm::mat4 GetViewMatrix();
private:
	float Zoom;
};

#endif
