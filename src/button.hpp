#include<SFML/Graphics.hpp>
#include<string>


#pragma once

class Button
{
	friend class Gomoku;
public:
	Button():window(NULL),isActive(false),PreBehRel(false) {};
	bool isActive;
	bool PreBehRel;
	void setTextrue(std::string str){
        this->t.loadFromFile(str);
	    this->s.setTexture(this->t);
    }

	void setPosition(int x, int y){
        this->s.setPosition(x, y);
    }

	void setScale(float x, float y){
        this->s.setScale(x, y);
    }

	void show(){
        isActive = true;
        (*window).draw(s);
    }

	bool onClick(sf::Event& e){
        if(!isActive) return false;  
        bool flag = false;
        sf::FloatRect box  = s.getGlobalBounds();
        if (sf::Mouse::getPosition(*window).x >= box.left && sf::Mouse::getPosition(*window).x <= (box.left + box.width) && sf::Mouse::getPosition(*window).y >= box.top && sf::Mouse::getPosition(*window).y <= (box.top + box.height)){
            if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left && PreBehRel){
                this->PreBehRel = false;
                flag = true;
            }else{
                flag = false;
            }
            if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left){
                this->s.setColor(sf::Color(125, 125, 0, 255));
                this->PreBehRel = true;
            }else{
                this->s.setColor(sf::Color(125, 125, 0, 100));
            }
        }
        else{
            this->s.setColor(sf::Color(255, 255, 255, 255));
        }
        return flag;
    }

	sf::Sprite s;//精灵
private:
	sf::Texture t;//贴图
	sf::RenderWindow* window;
};