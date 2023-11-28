#pragma once

class Component {
private:
public:
    Component();
    virtual ~Component();
};

class Entity {
private:
public:
    Entity();
    virtual ~Entity();
};

class System {
private:
public:
    System();
    virtual ~System();
};

class Registry {
private:
public:
    Registry();
    virtual ~Registry();
};