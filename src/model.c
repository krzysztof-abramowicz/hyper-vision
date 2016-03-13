
#include "model.h"
#include "model_p.h"

//TODO: run-time Wavefront OBJ model loader

//: HV::Model constructors and destructor

model_t hvCreateStockModel(enum smodel_e modelIndex)
{
	return hvCreateModel(
		stockModel[modelIndex]->name,
		stockModel[modelIndex]->vertices, stockModel[modelIndex]->vsize,
	    stockModel[modelIndex]->indices,  stockModel[modelIndex]->isize);
}

model_t hvCreateModel(const char* name, vtx3_t* vertices, size_t vsize, GLushort* indices, size_t isize)
{
	/* 1. Create a new model record with a mesh buffer (CPU-side) */
	model_t model = {{0}, {{0}}, {{0}}, NULL};
		mesh_t* mesh   = (mesh_t*) malloc(sizeof(mesh_t));
		strcpy(mesh->name, name);
		mesh->vertices = (vtx3_t*) malloc(sizeof(vtx3_t) * vsize);
		mesh->indices  = (GLushort*) malloc(sizeof(GLushort) * isize);
		memcpy(mesh->vertices, vertices, sizeof(vtx3_t) * vsize);
		memcpy(mesh->indices, indices, sizeof(GLushort) * isize);
		mesh->vsize = vsize;
		mesh->isize = isize;
	model.color = (col3_t){{.5f, .5f, .5f}};
	model.mesh = mesh;
	model.matrix = HV_IDENTITY_MATRIX;

	/* 2. Define the Vertex Array Object for model's data (GPU-side) */
	glGenVertexArrays(1, &model.buffer[HV_VERTEX_ARRAY]);
	glBindVertexArray(model.buffer[HV_VERTEX_ARRAY]);
	hvErrorCheckGL("Could not create the Vertex Array Object");

		/* Generate buffers for model's vertices and indices */
		glGenBuffers(2, &model.buffer[HV_BUFFERS]);
		hvErrorCheckGL("Could not generate VBO/IBO buffers for model");

		/* Bind and fill the Vertex Buffer Object (VBO) */
		glBindBuffer(GL_ARRAY_BUFFER, model.buffer[HV_VERTEX_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, vsize * sizeof(vtx3_t), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, HV_XYZ, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*) 0);
		glVertexAttribPointer(1, HV_XYZ, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*) sizeof(vertices[0].pos));
		hvErrorCheckGL("Could not construct Vertex Buffer Object for model");

		/* Bind and fill the Index Buffer Object (IBO) */
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.buffer[HV_INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, isize * sizeof(GLushort), indices, GL_STATIC_DRAW);
		hvErrorCheckGL("Could not construct Index Buffer Object for model");

		/* Enable vertex attributes */ 
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		hvErrorCheckGL("Could not enable vertex attributes");

	glBindVertexArray(0);

	return model;
}

void hvDeleteModel(model_t *model)
{
	glDeleteBuffers(2, &model->buffer[HV_BUFFERS]);
	glDeleteVertexArrays(1, &model->buffer[HV_VERTEX_ARRAY]);
	hvErrorCheckGL("Could not destroy buffer objects");

	if (model->mesh) {
		free(model->mesh->vertices);
		free(model->mesh->indices);
		free(model->mesh);
	}
}

//: HV::Model accessors (getters)

const char * hvModelName(model_t* model)
{
	return model->mesh->name;
}

size_t hvModelFaces(model_t* model) {
	size_t isize = model->mesh->isize;
	assert(isize % 3 == 0); //Model's mesh contains torn triangles.
	return isize / 3;
}

//: HV::Model accessors (setters)

void hvModelColor(model_t* model, col3_t color) {
	model->color = color;
}

//: Mesh transformations

void hvModelFaceFlip(model_t *model) {
	// mirror triangles
	#ifdef HV_ASSEMBLY_HACKS
		/* mot: fast swap of two 16-bit values with ROL */
		GLushort * ind = model->mesh->indices + 1;
		GLushort * end = ind + model->mesh->isize;
		__asm__ (
			"1:\n\t"
			"movl (%%rbx), %%eax\n\t"
			"rol $16, %%eax\n\t"
			"movl %%eax, (%%rbx)\n\t"
			"addq $6, %%rbx\n\t"
			"cmpq %%rbx, %%rdx\n\t"
			"ja 1b\n\t"
			: : "b"(ind), "d"(end) : "%eax", "cc"
		);
	#else
		GLushort tmp;
		GLushort * ind = model->mesh->indices;
		for (size_t i = 0; i < model->mesh->isize; i += 3) {
			tmp = ind[i+1];
			ind[i+1] = ind[i+2];
			ind[i+2] = tmp;
		}
	#endif
	// negate normals
	vtx3_t *vtx = model->mesh->vertices;
	for (size_t i = 0; i < model->mesh->vsize; ++i) {
		vtx[i].nor[X] = -vtx[i].nor[X];
		vtx[i].nor[Y] = -vtx[i].nor[Y];
		vtx[i].nor[Z] = -vtx[i].nor[Z];
	}
}//TODO: provide SSE version

void hvModelFaceRoll(model_t *model) {
	GLushort tmp;
	// rotate indices
	GLushort *ind = model->mesh->indices;
	for (size_t i = 0; i < model->mesh->isize; i += 3) {
		tmp = ind[i];
		ind[i] = ind[i+1];
		ind[i+1] = ind[i+2];
		ind[i+2] = tmp;
	}
}

//: Model operations

void hvModelSync(model_t* model)
{
	mesh_t* mesh = model->mesh;
	glBindVertexArray(model->buffer[HV_VERTEX_ARRAY]);
		glBindBuffer(GL_ARRAY_BUFFER, model->buffer[HV_VERTEX_BUFFER]);
			glBufferData(GL_ARRAY_BUFFER, mesh->vsize * sizeof(vtx3_t), mesh->vertices, GL_STATIC_DRAW);
			hvErrorCheckGL("Could not bind VBO to the Vertex Array Object");
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->buffer[HV_INDEX_BUFFER]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->isize * sizeof(GLuint), mesh->indices, GL_STATIC_DRAW);
			hvErrorCheckGL("Could not bind IBO to the Vertex Array Object");
	glBindVertexArray(0);
}

void hvModelDraw(model_t* model, prog_t* prog, mat4_t* viewMatrix)
{
	glUseProgram(prog->id);
		glUniform3fv(prog->colorUniLoc, 1, model->color.v);
		glUniformMatrix4fv(prog->mdelMatrixUniLoc, 1, GL_FALSE, model->matrix.m);
		glUniformMatrix4fv(prog->viewMatrixUniLoc, 1, GL_FALSE, viewMatrix->m);
		hvErrorCheckGL("Could not set the shader programme uniforms");
		glBindVertexArray(model->buffer[HV_VERTEX_ARRAY]);
			glDrawElements(GL_TRIANGLES, model->mesh->isize, GL_UNSIGNED_SHORT, (GLvoid*)0);
			hvErrorCheckGL("Could not draw the model");
		glBindVertexArray(0);
	glUseProgram(0);
}

void hvModelDump(model_t* model)
{
	mesh_t* mesh = model->mesh;
	printf("  DEBUG: \"%s\" model dump\n", mesh->name);
	for (size_t i = 0; i < mesh->vsize; ++i)
		printf(
			"    (%4.1f %4.1f %4.1f)  (%4.1f %4.1f %4.1f)\n",
			mesh->vertices[i].pos[0], mesh->vertices[i].pos[1], mesh->vertices[i].pos[2],
			mesh->vertices[i].nor[0], mesh->vertices[i].nor[1], mesh->vertices[i].nor[2]
		);
}

