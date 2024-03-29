#pragma once

#include <functional>
#include <scl/string.hpp>
#include <scl/vector.hpp>

#include "../core/FunctionMap.hpp"
#include "../core/HeterogenMap.hpp"
#include "JointProcessor.hpp"

class PhysicBodyBase {
    friend class PhysicSimulation;

public:
    void add_update(const std::string& name, std::function<void(PhysicBodyBase&, double)>&& callback) {
        _update_functions.emplace_back(name, std::move(callback));
    }

    void remove_update(const std::string& name) {
        _update_functions.erase(name);
    }

    scl::Vector<scl::String> get_update_list() const {
        scl::Vector<scl::String> vec;

        _update_functions.foreach([&vec](const std::string& name) {
            vec.emplace_back(name);
        });

        return vec;
    }

    bool is_update_exists(const std::string& name) const {
        return _update_functions.lookup(name);
    }

    auto& user_data() {
        return _user_data;
    }

    auto& user_data() const {
        return _user_data;
    }

protected:
    void setWorld(class b2World* world) {_world = world; }
    virtual void destroy() = 0;

    class b2World* _world = nullptr;

    FunctionMap<void(PhysicBodyBase&, double)> _update_functions;
    HeterogenMap<std::string>                  _user_data;
};


class SimpleBody : public PhysicBodyBase {
    friend class PhysicSimulation;
public:
    SimpleBody(class b2World* world, class b2Body* body) {
        _world = world;
        _body = body;
    }

    void destroy() override;

protected:
    class b2Body* _body;
};


class BodyWithJoints : public PhysicBodyBase {
    friend class PhysicSimulation;

public:
    virtual class b2Joint* get_joint(int joint_index) const = 0;

    template <typename T, typename... ArgsT>
    auto joint_processor_new(const std::string& name, int joint_index, ArgsT&&... args) {
        return _jpm.create<T>(name, get_joint(joint_index), args...);
    }

    void remove_joint_processor(const std::string& name) {
        _jpm.erase(name);
    }

    auto joint_processor_get(const std::string& name) const {
        return _jpm.get(name);
    }

    template <typename T>
    auto joint_processor_cast_get(const std::string& name) const {
        return _jpm.cast_get<T>(name);
    }

    scl::Vector<scl::String> joint_processors_list() const {
        scl::Vector<scl::String> res;
        for (auto& p : _jpm.data())
            res.emplace_back(p.first);

        return res;
    }

    bool is_joint_processor_exists(const std::string& name) const {
        return _jpm.data().find(name) != _jpm.data().end();
    }

protected:
    JointProcessorManager _jpm;
};