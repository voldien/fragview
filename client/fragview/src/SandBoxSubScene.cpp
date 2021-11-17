#include "SandBoxSubScene.h"
#include <Scene/Scene.h>

#include <Buffer.h>
#include <FrameBuffer.h>
#include <Query.h>
#include <Sync.h>
#include <ViewPort.h>

using namespace fragcore;
using namespace fragengine;
using namespace fragview;

void SandBoxSubScene::addTexture(Texture *texture) {
	this->textures.push_back(texture);
	this->computeMapTargets.push_back(Texture::MapTarget::eRead);
	this->computeFormats.push_back(Texture::Format::eR8G8B8A8);
	texture->increment();
}

void SandBoxSubScene::addShader(Shader *Shader) {
	/*  Validate the shader has the correct attributes. */
	if (!Shader->getShader(Shader::FRAGMENT_SHADER))
		throw InvalidArgumentException("Invalid pipeline. Requires a fragment component");

	/*  Update the uniform locations.   */
	UniformLocation location;
	updateUniformLocations(Shader, &location, Shader::FRAGMENT_SHADER);
	this->cachedUniforms[Shader->getUID()] = location; //.insert(this->cachedUniforms.begin(), location);

	// Update references.
	this->sequences[Shader::FRAGMENT_SHADER].push_back(Shader);
	Shader->increment();
}

void SandBoxSubScene::updateUniformLocations(Shader *Shader, UniformLocation *location,
											 Shader::ShaderType shaderStage) {

	/*  Get and cache uniform locations.    */
	location->time = Shader->getShader(shaderStage)->getLocation("time");
	location->deltatime = Shader->getShader(shaderStage)->getLocation("deltatime");
	location->resolution = Shader->getShader(shaderStage)->getLocation("resolution");
	location->position = Shader->getShader(shaderStage)->getLocation("position");
	location->mouse = Shader->getShader(shaderStage)->getLocation("mouse");
	location->movement = Shader->getShader(shaderStage)->getLocation("movement");
	location->offset = Shader->getShader(shaderStage)->getLocation("offset");
	location->backbuffer = Shader->getShader(shaderStage)->getLocation("backbuffer");
	location->pointer = Shader->getShader(shaderStage)->getLocation("pointer");

	int tex0 = Shader->getShader(shaderStage)->getLocation("texture0");
	Shader->getShader(shaderStage)->setInt(tex0, 0);

	/*  Log only in debug mode. */
	Log::log(Log::Debug, "----------- fetching uniforms index location ----------\n");
	Log::log(Log::Debug, "time %d\n", location->time);
	Log::log(Log::Debug, "deltatime %d\n", location->deltatime);
	Log::log(Log::Debug, "resolution %d\n", location->resolution);
	Log::log(Log::Debug, "mouse %d\n", location->mouse);
	Log::log(Log::Debug, "offset %d\n", location->offset);
	Log::log(Log::Debug, "stdin %d\n", location->stdin);
	Log::log(Log::Debug, "backbuffer %d\n", location->backbuffer);
}

void SandBoxSubScene::addCompute(Shader *compute) {
	if (!compute->getShader(Shader::COMPUTE_SHADER))
		throw InvalidArgumentException("Invalid pipeline. Requires a fragment component");

	this->sequences[Shader::COMPUTE_SHADER].push_back(compute);

	UniformLocation location;
	updateUniformLocations(compute, &location, Shader::COMPUTE_SHADER);
	this->cachedUniforms[compute->getUID()] = location; //.insert(this->cachedUniforms.begin(), location);

	unsigned int localSize[3];
	compute->getShader(Shader::COMPUTE_SHADER)->getLocalGroupSize(&localSize[0]);
	// this->computeLocal.push_back(localSize);

	compute->increment();
}

void SandBoxSubScene::setDisptchBuffer(Ref<Buffer> dispatchBuffer) {
	this->indirectBuffer = dispatchBuffer;
	assert(*dispatchBuffer);
}

Texture *SandBoxSubScene::getTexture(int index) { return this->textures[index]; }

Shader *SandBoxSubScene::getShader(int index) { return this->sequences[Shader::FRAGMENT_SHADER][index]; }

Shader *SandBoxSubScene::getCompute(int index) { return this->sequences[Shader::COMPUTE_SHADER][index]; }

int SandBoxSubScene::getNumTextures() const { return this->textures.size(); }

int SandBoxSubScene::getNumShaders() const { return this->sequences[Shader::FRAGMENT_SHADER].size(); }

int SandBoxSubScene::getNumCompute() const { return this->sequences[Shader::COMPUTE_SHADER].size(); }

const UniformLocation *SandBoxSubScene::getUniformLocation(int uid) const { return &this->cachedUniforms[uid]; }

UniformLocation *SandBoxSubScene::getUniformLocation(int uid) { return &this->cachedUniforms[uid]; }

FragGraphicUniform *SandBoxSubScene::getFragUniform() { return &this->uniforms; }

const std::vector<Texture::MapTarget> &SandBoxSubScene::getComputeMapTargets() const { return this->computeMapTargets; }

const std::vector<Texture::Format> &SandBoxSubScene::getComputeFormats() const { return this->computeFormats; }

const int *SandBoxSubScene::getComputeLocalWorkGroup(int index) {}

void SandBoxSubScene::updateAllUniformLocations() {

	std::map<Shader::ShaderType, std::vector<Shader *>>::iterator it = this->sequences.begin();
	for (; it != this->sequences.end(); it++) {
		Shader::ShaderType shaderType = (*it).first;
		std::vector<Shader *> programs = (*it).second;
		for (int i = 0; i < programs.size(); i++) {

			Shader *Shader = programs[i];
			UniformLocation *uniformLocation = getUniformLocation(Shader->getUID());
			this->updateUniformLocations(Shader, uniformLocation, shaderType);
		}
	}
}

SandBoxSubScene::~SandBoxSubScene() { /*  Decrease references. */
}

const std::vector<Texture *> &SandBoxSubScene::getTextures() const { return this->textures; }

//
////TODO add for fetching the local variable size of the local group.
////TODO see if can be set manually.
///*  Update the compute dispatch buffer.*/
// IndirectDispatch dispatch = {
//		.num_groups_x = (unsigned int) (width / 16),
//		.num_groups_y = (unsigned int) (height / 16),
//		.num_groups_z = 1
//};
////TODO relocate to sandbox scene object.
// void *dispatchBuf = this->computeIndirect->mapBuffer(Buffer::MapTarget::eWrite | Buffer::MapTarget::eNoSync, 0,
//                                                     sizeof(IndirectDispatch));
////IndirectDispatch* dispatch = (IndirectDispatch*)dispatchBuf;
// for(int i = 0; i < 0; i++){
//
//}
// memcpy(dispatchBuf, &dispatch, sizeof(dispatch));
// this->computeIndirect->unMapBuffer();
