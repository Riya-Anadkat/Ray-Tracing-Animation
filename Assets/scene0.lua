
glass_blue = gr.material({0.3, 0.5, 0.8}, {0.8, 0.8, 0.9}, 50)  -- Blue glass buildings
glass_dark = gr.material({0.2, 0.2, 0.3}, {0.7, 0.7, 0.8}, 40)  -- Dark glass buildings
concrete = gr.material({0.6, 0.6, 0.6}, {0.3, 0.3, 0.3}, 10)    -- Concrete buildings
gold_top = gr.material({0.9, 0.9, 0.7}, {1.0, 1.0, 0.8}, 150)    -- Gold-topped buildings (closer to white, more shiny metallic)
ground = gr.material({0.3, 0.5, 0.3}, {0.4, 0.6, 0.4}, 30)       -- Ground/grass (slightly shinier)
dark_concrete = gr.material({0.4, 0.4, 0.4}, {0.2, 0.2, 0.2}, 10)  -- Dark concrete buildings

glass_light1 = gr.material({80/255, 106/255, 200/255}, {0.8, 0.8, 0.9}, 50, 0.0, 1.5)  -- Light glass buildings
glass_light2 = gr.material({90/255, 116/255, 210/255}, {0.8, 0.8, 0.9}, 50, 0.0, 1.5)
glass_light3 = gr.material({100/255, 126/255, 220/255}, {0.8, 0.8, 0.9}, 50, 0.0, 1.5)
glass_light4 = gr.material({110/255, 136/255, 230/255}, {0.8, 0.8, 0.9}, 50, 0.0, 1.5)
glass_light5 = gr.material({120/255, 146/255, 240/255}, {0.8, 0.8, 0.9}, 50, 0.0, 1.5)
glass_light6 = gr.material({130/255, 156/255, 250/255}, {0.8, 0.8, 0.9}, 50, 0.0, 1.5)
glass_light7 = gr.material({140/255, 166/255, 255/255}, {0.8, 0.8, 0.9}, 50, 0.0, 1.5)
glass_light8 = gr.material({150/255, 176/255, 255/255}, {0.8, 0.8, 0.9}, 50, 0.0, 1.5)
glass_light9 = gr.material({160/255, 186/255, 255/255}, {0.8, 0.8, 0.9}, 50, 0.0, 1.5)
glass_light10 = gr.material({170/255, 196/255, 255/255}, {0.8, 0.8, 0.9}, 50, 0.0, 1.5)
glass_light11 = gr.material({160/255, 200/255, 255/255}, {0.8, 0.8, 0.9}, 50, 0.0, 1.5)

blue_light = gr.material({0.2, 0.4, 1.0}, {0.5, 0.5, 0.5}, 50, 0.0, 1.5)  -- Blue light buildings
black_light = gr.material({0.1, 0.1, 0.1}, {0.5, 0.5, 0.5}, 50, 0.0, 1.5)  -- Black light buildings
grey_light = gr.material({0.3, 0.3, 0.3}, {0.2, 0.2, 0.2}, 10, 0.0, 1.5)  -- Grey matte buildings
yellow_light = gr.material({0.9, 0.9, 0.1}, {0.2, 0.2, 0.2}, 10, 0.0, 1.5)  -- Yellow matte buildings
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)
red_light = gr.material({0.6, 0.1, 0.1}, {0.4, 0.4, 0.4}, 50, 0.0, 1.5)  -- Dark red light buildings
light_grey_light = gr.material({0.8, 0.8, 0.8}, {0.5, 0.5, 0.5}, 50, 0.0, 1.5)  -- Light grey light buildings

scene_root = gr.node('root')
scene_root:translate(0, 0, -3)


plane = gr.mesh( 'plane', 'plane.obj' )
scene_root:add_child(plane)
plane:set_material(concrete)
plane:scale(500, 1, 1000)
plane:translate(0, -30, -100)

road = gr.mesh('road', 'plane.obj')
scene_root:add_child(road)
road:set_material(dark_concrete)
road:scale(17, 1, 1000)
road:translate(0, -29, -100)

b1 = gr.nh_box('b1', {-45, -20, 720}, 20)
b1:scale(1.0, 2.0, 1.0)
scene_root:add_child(b1)
b1:set_material(glass_light1)

b2 = gr.nh_box('b2', {20, -10, 735}, 15)
b2:scale(1.0, 2.4, 1.0)
scene_root:add_child(b2)
b2:set_material(glass_light1)

b3 = gr.nh_box('b3', {15, -20, 700}, 30)
b3:scale(1.0, 0.8, 1.0)
scene_root:add_child(b3)
b3:set_material(glass_light2)

b4 = gr.nh_box('b4', {200, -4, 300}, 45)
b4:scale(1.0, 5.0, 1.0)
scene_root:add_child(b4)
b4:set_material(glass_light5)

b5 = gr.nh_box('b5', {40, -20, 470}, 55)
b5:scale(1.0, 2.0, 1.0)


scene_root:add_child(b5)
b5:set_material(glass_light8)

b6 = gr.nh_box('b6', {-70, -30, 600}, 45)
b6:scale(1.0, 0.9, 1.0)
scene_root:add_child(b6)
b6:set_material(glass_light6)

b21 = gr.nh_box('b21', {70, -6, 200}, 35)
b21:scale(1.0, 8.0, 1.0)
scene_root:add_child(b21)
b21:set_material(glass_light5)

b22 = gr.nh_box('b22', {-50, -10, 450}, 30)
b22:scale(1.3, 3.0, 1.0)
scene_root:add_child(b22)
b22:set_material(glass_light2)

b23 = gr.nh_box('b23', {-250, -10, 350}, 45)
b23:scale(1.0, 4.0, 1.0)
scene_root:add_child(b23)
b23:set_material(glass_light3)

b24 = gr.nh_box('b24', {-50, -30, 680}, 30)
b24:scale(1.0, 0.9, 1.0)
scene_root:add_child(b24)
b24:set_material(glass_light6)

b25 = gr.nh_box('b25', {-260, -10, 50}, 55)
b25:scale(1.0, 7, 1.0)
scene_root:add_child(b25)
b25:set_material(glass_light5)

for j = 30, 50 do
	local i = j - 20
	local x = -500 + (i - 10) * 80
	local y = -((i - 14) >= 0 and (i - 14) or -(i - 14)) - 3
	local scale_y = 5 + (i % 3)
	local box = gr.nh_box('b' .. j, {x, y, -350}, 50)
	box:scale(1.0, scale_y, 1.0)
	scene_root:add_child(box)
	box:set_material(glass_light10)
end

-- scar1 = gr.mesh('scar1', 'car.obj')
-- scene_root:add_child(scar1)
-- scar1:set_material(yellow_light)
-- -- -- at 0
-- scar1:translate(143, -200, 950)
-- -- at 1
-- -- scar1:translate(143, -200, 650)
-- -- at 2
-- -- scar1:translate(143, -200, 850)
-- --at 2.5 seconds
-- -- scar1:translate(143, -200, 900)


-- scar1:scale(0.55,0.55,0.75)

-- scar2 = gr.mesh('scar2', 'car.obj')
-- scene_root:add_child(scar2)
-- scar2:set_material(light_grey_light)
-- scar2:translate(143, -200, 937)
-- scar2:scale(0.55,0.55,0.75)

-- scar3 = gr.mesh('scar3', 'car.obj')
-- scene_root:add_child(scar3)
-- scar3:set_material(red_light)
-- scar3:translate(143, -200, 919)
-- scar3:scale(0.55,0.55,0.75)


-- scar4 = gr.mesh('scar4', 'car.obj')
-- scene_root:add_child(scar4)
-- scar4:set_material(blue_light)
-- scar4:translate(143, -200, 1005)
-- scar4:scale(0.55,0.55,0.75)

star1 = gr.mesh('star1', 'smstdodeca.obj')
scene_root:add_child(star1)
star1:set_material(gold_top)
star1:translate(0, 2000, -20)
star1:scale(0.1, 0.1, 0.1)


star2 = gr.mesh('star2', 'smstdodeca.obj')
scene_root:add_child(star2)
star2:set_material(gold_top)
star2:translate(600, 2800, -20)
star2:scale(0.1, 0.1, 0.1)

star3 = gr.mesh('star3', 'smstdodeca.obj')
scene_root:add_child(star3)
star3:set_material(gold_top)
star3:translate(1000, 2200, -20)
star3:scale(0.1, 0.1, 0.1)

star4 = gr.mesh('star4', 'smstdodeca.obj')
scene_root:add_child(star4)
star4:set_material(gold_top)
star4:translate(-1600, 3200, -20)
star4:scale(0.1, 0.1, 0.1)

star5 = gr.mesh('star5', 'smstdodeca.obj')
scene_root:add_child(star5)
star5:set_material(gold_top)
star5:translate(-3100, 2900, -20)
star5:scale(0.1, 0.1, 0.1)

star6 = gr.mesh('star6', 'smstdodeca.obj')
scene_root:add_child(star6)
star6:set_material(gold_top)
star6:translate(2500, 2200, -20)
star6:scale(0.1, 0.1, 0.1)

star7 = gr.mesh('star7', 'smstdodeca.obj')
scene_root:add_child(star7)
star7:set_material(gold_top)
star7:translate(2900, 2800, -20)
star7:scale(0.1, 0.1, 0.1)


star8 = gr.mesh('star8', 'smstdodeca.obj')
scene_root:add_child(star8)
star8:set_material(gold_top)
star8:translate(-1900, 2500, -20)
star8:scale(0.1, 0.1, 0.1)

star9 = gr.mesh('star9', 'smstdodeca.obj')
scene_root:add_child(star9)
star9:set_material(gold_top)
star9:translate(-900, 2700, -20)
star9:scale(0.1, 0.1, 0.1)

star10 = gr.mesh('star10', 'smstdodeca.obj')
scene_root:add_child(star10)
star10:set_material(gold_top)
star10:translate(-500, 3200, -20)
star10:scale(0.1, 0.1, 0.1)

star11 = gr.mesh('star11', 'smstdodeca.obj')
scene_root:add_child(star11)
star11:set_material(gold_top)
star11:translate(2100, 3300, -20)
star11:scale(0.1, 0.1, 0.1)

white_light = gr.light({-100.0, -30.0, 750.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

sun_lights = {}
for i = 1, 120 do
	local intensity = 0.5 - (i - 1) * (0.5 / 119)  -- Gradually decrease intensity
	local color = {intensity, intensity * 0.5, 0.0}  -- Adjust color to simulate sunset
	sun_lights[i] = gr.light({-750.0, 30.0, 900.0}, color, {1, 0, 0})
end


gr.render(scene_root, 'scene/frame_0231.png', 500, 500,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, sun_lights[120]})
