#include "SandBoxSubScene.h"
#include <Exception/InvalidArgumentException.h>
#include <Scene/Scene.h>

#include <Renderer/Buffer.h>
#include <Renderer/FrameBuffer.h>
#include <Renderer/ProgramPipeline.h>
#include <Renderer/Query.h>
#include <Renderer/Sync.h>
#include <Renderer/ViewPort.h>

using namespace fragcore;
using namespace fragengine;
using namespace fragview;

void SandBoxSubScene::addTexture(Texture *texture) {
	this->textures.push_back(texture);
	this->computeMapTargets.push_back(Texture::MapTarget::eRead);
	this->computeFormats.push_back(Texture::Format::eR8G8B8A8);
	texture->increment();
}

void SandBoxSubScene::addShader(ProgramPipeline *programPipeline) {
	/*  Validate the shader has the correct attributes. */
	if (!programPipeline->getShader(ProgramPipeline::FRAGMENT_SHADER))
		throw InvalidArgumentException("Invalid pipeline. Requires a fragment component");

	/*  Update the uniform locations.   */
	UniformLocation location;
	updateUniformLocations(programPipeline, &location, ProgramPipeline::FRAGMENT_SHADER);
	this->cachedUniforms[programPipeline->getUID()] = location; //.insert(this->cachedUniforms.begin(), location);

	// Update references.
	this->sequences[ProgramPipeline::FRAGMENT_SHADER].push_back(programPipeline);
	programPipeline->increment();
}

void SandBoxSubScene::updateUniformLocations(ProgramPipeline *programPipeline, UniformLocation *location,
											 ProgramPipeline::ShaderType shaderStage) {

	/*  Get and cache uniform locations.    */
	location->time = programPipeline->getShader(shaderStage)->getLocation("time");
	location->deltatime = programPipeline->getShader(shaderStage)->getLocation("deltatime");
	location->resolution = programPipeline->getShader(shaderStage)->getLocation("resolution");
	location->position = programPipeline->getShader(shaderStage)->getLocation("position");
	location->mouse = programPipeline->getShader(shaderStage)->getLocation("mouse");
	location->movement = programPipeline->getShader(shaderStage)->getLocation("movement");
	location->offset = programPipeline->getShader(shaderStage)->getLocation("offset");
	location->backbuffer = programPipeline->getShader(shaderStage)->getLocation("backbuffer");
	location->pointer = programPipeline->getShader(shaderStage)->getLocation("pointer");

	int tex0 = programPipeline->getShader(shaderStage)->getLocation("texture0");
	programPipeline->getShader(shaderStage)->setInt(tex0, 0);

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

void SandBoxSubScene::addCompute(ProgramPipeline *compute) {
	if (!compute->getShader(ProgramPipeline::COMPUTE_SHADER))
		throw InvalidArgumentException("Invalid pipeline. Requires a fragment component");

	this->sequences[ProgramPipeline::COMPUTE_SHADER].push_back(compute);

	UniformLocation location;
	updateUniformLocations(compute, &location, ProgramPipeline::COMPUTE_SHADER);
	this->cachedUniforms[compute->getUID()] = location; //.insert(this->cachedUniforms.begin(), location);

	unsigned int localSize[3];
	compute->getShader(ProgramPipeline::COMPUTE_SHADER)->getLocalGroupSize(&localSize[0]);
	// this->computeLocal.push_back(localSize);

	compute->increment();
}

void SandBoxSubScene::setDisptchBuffer(Ref<Buffer> dispatchBuffer) {
	this->indirectBuffer = dispatchBuffer;
	assert(*dispatchBuffer);
}

Texture *SandBoxSubScene::getTexture(int index) { return this->textures[index]; }

ProgramPipeline *SandBoxSubScene::getShader(int index) {
	return this->sequences[ProgramPipeline::FRAGMENT_SHADER][index];
}

ProgramPipeline *SandBoxSubScene::getCompute(int index) {
	return this->sequences[ProgramPipeline::COMPUTE_SHADER][index];
}

int SandBoxSubScene::getNumTextures(void) const { return this->textures.size(); }

int SandBoxSubScene::getNumShaders(void) const { return this->sequences[ProgramPipeline::FRAGMENT_SHADER].size(); }

int SandBoxSubScene::getNumCompute(void) const { return this->sequences[ProgramPipeline::COMPUTE_SHADER].size(); }

const UniformLocation *SandBoxSubScene::getUniformLocation(int uid) const { return &this->cachedUniforms[uid]; }

UniformLocation *SandBoxSubScene::getUniformLocation(int uid) { return &this->cachedUniforms[uid]; }

FragGraphicUniform *SandBoxSubScene::getFragUniform(void) { return &this->uniforms; }

const std::vector<Texture::MapTarget> &SandBoxSubScene::getComputeMapTargets(void) const {
	return this->computeMapTargets;
}

const std::vector<Texture::Format> &SandBoxSubScene::getComputeFormats(void) const { return this->computeFormats; }

const int *SandBoxSubScene::getComputeLocalWorkGroup(int index) {}

void SandBoxSubScene::updateAllUniformLocations(void) {

	std::map<ProgramPipeline::ShaderType, std::vector<ProgramPipeline *>>::iterator it = this->sequences.begin();
	for (; it != this->sequences.end(); it++) {
		ProgramPipeline::ShaderType shaderType = (*it).first;
		std::vector<ProgramPipeline *> programs = (*it).second;
		for (int i = 0; i < programs.size(); i++) {

			ProgramPipeline *programPipeline = programs[i];
			UniformLocation *uniformLocation = getUniformLocation(programPipeline->getUID());
			this->updateUniformLocations(programPipeline, uniformLocation, shaderType);
		}
	}
}

SandBoxSubScene::~SandBoxSubScene(void) { /*  Decrease references. */ }

const std::vector<Texture *> &SandBoxSubScene::getTextures(void) const { return this->textures; }

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
