#include "Renderer.h"
#include "ParticleSystem.h"

int main()
{
	ParticleSystem system;
	system.Start();

	Renderer r(&system);

	return 0;
}
