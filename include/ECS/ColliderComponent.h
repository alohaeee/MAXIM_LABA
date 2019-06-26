#pragma once

#include "Components.h"
#include <SDL2/SDL.h>
#include "Collision.h"
#include <string>
#include <functional>
#include <tuple>

using COLLISON_FUNC = std::function<bool(ColliderComponent&,ColliderComponent&)>;
using COLL_HANDLER_FUNC = std::function<void(Entity&,Entity&)>;

using COLLIDER_UPDATE_FUNC = std::function<void(ColliderComponent&)>;
namespace ColliderUpdateFunc
{
    void TransfromBasicCollider(ColliderComponent &CollComp);
    void TransfromFISHCollider(ColliderComponent &CollComp);
    void TransfromHOOKCollider(ColliderComponent &CollComp);
};

struct ColliderComponent : public Component
{
    SDL_Rect collider;
    TransformComponent* transform;
    SpriteComponent *sprite;
    
    std::string tag;
    bool debug = true;
    

    std::vector<std::tuple<std::string,COLLISON_FUNC,COLL_HANDLER_FUNC> > coll_pair;
    
    static std::map<std::string,std::reference_wrapper<ColliderComponent>> colliders;

    COLLIDER_UPDATE_FUNC CUF = ColliderUpdateFunc::TransfromBasicCollider;

    ColliderComponent(std::string tag) : tag(tag) {}
    ColliderComponent(std::string tag, COLLIDER_UPDATE_FUNC update_func) : tag(tag), CUF(update_func) {}
    
    void init() override
    {
        if(!entity->hasComponent<TransformComponent>())
        {
            entity->addComponent<TransformComponent>();
        }
        transform = &entity->getComponent<TransformComponent>();
        sprite = &entity->getComponent<SpriteComponent>();

        colliders.emplace(tag,*this);
    }

    void update() override
    {   
        CUF(*this);
        for(auto &cp : coll_pair)
        {
            auto c = colliders.at(std::get<std::string>(cp)).get();
            if(std::get<COLLISON_FUNC>(cp)(*this,c))
                std::get<COLL_HANDLER_FUNC>(cp)(*entity,*c.entity);
        }
    }

    void draw() override
    {
        if(debug)
        {
            SDL_RenderDrawRect(Game::renderer,&collider);
        }
        
    }

    //@Make Collision Pair for *this ColliderComponent@
    //std::string col_tag - ColliderComponent's tag with which making Pair
    //COLLISON_FUNC col_func - Collision detecting function 
    //COLL_HANDLER_FUNC hand_func - Function for handling collision
    void MakeCollPair(std::string col_tag, COLLISON_FUNC col_func, COLL_HANDLER_FUNC hand_func)
    {
        coll_pair.push_back(std::make_tuple(col_tag, col_func,hand_func));
    }
};

