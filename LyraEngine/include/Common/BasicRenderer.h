/*************************
 * @file BasicRenderer.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief A base implementation for render systems interacting with render objects
 * 
 * @date 2023-08-23
 * 
 * @copyright Copyright (c) 2023
 *************************/

#pragma once

#include <Common/Common.h>
#include <LSD/UnorderedSparseSet.h>

namespace lyra {

class BasicRenderer {
public:
	virtual void drawAll() const = 0;

protected:
	virtual void addRenderObject(RenderObject*) = 0;
	virtual void removeRenderObject(RenderObject*) = 0;

	friend class RenderObject;
};

class RenderObject {
public:
	RenderObject() = default;
	RenderObject(BasicRenderer* renderSystem) : m_renderSystem(renderSystem) {
		renderSystem->addRenderObject(this);
	}
	~RenderObject() {
		if (m_renderSystem) m_renderSystem->removeRenderObject(this);
	}

	virtual void draw() = 0;

protected:
	BasicRenderer* m_renderSystem;
};

class UnorderedBasicRenderer : public BasicRenderer {
public:
	void drawAll() const {
		for (auto& object : m_objects) {
			object->draw();
		}
	}

protected:
	void addRenderObject(RenderObject* o) {
		m_objects.insert(o);
	}
	void removeRenderObject(RenderObject* o) {
		m_objects.erase(o);
	}

	lsd::UnorderedSparseSet<RenderObject*> m_objects;
};

class VectorBasicRenderer : public BasicRenderer {
public:
	void drawAll() const {
		for (uint32 i = 0; i < m_objects.size(); i++) {
			m_objects[i]->draw();
		}
	}

protected:
	void addRenderObject(RenderObject* o) {
		m_objects.pushBack(o);
	}
	void removeRenderObject(RenderObject* o) {
		m_objects.erase(std::remove(m_objects.begin(), m_objects.end(), o), m_objects.end());
	}

	lsd::Vector<RenderObject*> m_objects;
};

} // namespace lyra
