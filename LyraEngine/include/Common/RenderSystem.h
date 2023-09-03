/*************************
 * @file RenderSystem.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief A base implementation for render systems interacting with render objects
 * 
 * @date 2023-08-23
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <deque>
#include <set>
#include <unordered_set>

namespace lyra {

class RenderSystem {
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
	RenderObject(RenderSystem* renderSystem) : m_renderSystem(renderSystem) {
		renderSystem->addRenderObject(this);
	}
	~RenderObject() {
		if (m_renderSystem) m_renderSystem->removeRenderObject(this);
	}

	virtual void draw() = 0;

protected:
	RenderSystem* m_renderSystem;
};

class UnorderedRenderSystem : public RenderSystem {
public:
	void drawAll() const final {
		for (auto& object : m_objects) {
			object->draw();
		}
	}

protected:
	void addRenderObject(RenderObject* o) final {
		m_objects.insert(o);
	}
	void removeRenderObject(RenderObject* o) final {
		m_objects.erase(o);
	}

	std::unordered_set<RenderObject*> m_objects;
};

class OrderedRenderSystem : public RenderSystem {
public:
	void drawAll() const final {
		for (auto& object : m_objects) {
			object->draw();
		}
	}

protected:
	void addRenderObject(RenderObject* o) final {
		m_objects.insert(o);
	}
	void removeRenderObject(RenderObject* o) final {
		m_objects.erase(o);
	}

	std::set<RenderObject*> m_objects;
};

class QueuedRenderSystem : public RenderSystem {
public:
	void drawAll() const final {
		for (auto it = m_objects.begin(); it != m_objects.end(); it++) {
			(*it)->draw();
		}
	}

protected:
	void addRenderObject(RenderObject* o) final {
		m_objects.push_back(o);
	}
	void removeRenderObject(RenderObject* o) final {
		m_objects.erase(std::remove(m_objects.begin(), m_objects.end(), o), m_objects.end());
	}

	std::deque<RenderObject*> m_objects;
};

} // namespace lyra
