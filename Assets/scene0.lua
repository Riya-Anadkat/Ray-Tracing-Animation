-- A New York City skyline scene made with boxes - Side View.
-- Based on the provided format with buildings representing skyscrapers

-- Materials for different building types
glass_blue = gr.material({0.3, 0.5, 0.8}, {0.8, 0.8, 0.9}, 50)  -- Blue glass buildings
glass_dark = gr.material({0.2, 0.2, 0.3}, {0.7, 0.7, 0.8}, 40)  -- Dark glass buildings
concrete = gr.material({0.7, 0.7, 0.7}, {0.3, 0.3, 0.3}, 10)    -- Concrete buildings
gold_top = gr.material({0.9, 0.8, 0.1}, {0.9, 0.8, 0.2}, 80)    -- Gold-topped buildings
ground = gr.material({0.2, 0.4, 0.2}, {0.1, 0.2, 0.1}, 5)       -- Ground/grass



scene = gr.node( 'scene' )
scene:rotate('X', 23)

-- plane = gr.mesh( 'plane', 'plane.obj' )
-- scene:add_child(plane)
-- plane:set_material(concrete)
-- plane:scale(30, 30, 30)


-- -- Various buildings of different heights
-- local building_positions = {
--     {-600, 180}, {-500, 230}, {-300, 150}, {-200, 280}, {-100, 120},
--     {100, 200}, {300, 250}, {400, 180}, {500, 350}, {600, 150},
--     {700, 220}, {800, 270}, {900, 120}, {1000, 200}, {1100, 150}
-- }

-- for i, pos in ipairs(building_positions) do
--     local x_pos = pos[1]
--     local height = pos[2]
--     local width = 40 + math.random(40)
    
--     local building = gr.nh_box('b'..i, {x_pos, height/2, 0}, width)
--     scene:add_child(building)
    
--     -- Alternate between materials
--     if i % 3 == 0 then
--         building:set_material(glass_blue)
--     elseif i % 3 == 1 then
--         building:set_material(glass_dark)
--     else
--         building:set_material(concrete)
--     end
-- end

-- -- === MID-RISE BUILDINGS (second row, slightly back) ===

-- for i=1,20 do
--     local x_pos = -800 + i * 90
--     local z_pos = 100  -- Offset slightly from main skyline
--     local height = 50 + math.random(100)
--     local width = 30 + math.random(30)
    
--     local building = gr.nh_box('m'..i, {x_pos, height/2, z_pos}, width)
--     scene:add_child(building)
    
--     -- Alternate between materials
--     if i % 4 == 0 then
--         building:set_material(glass_blue)
--     elseif i % 4 == 1 then
--         building:set_material(glass_dark)
--     else
--         building:set_material(concrete)
--     end
-- end

-- -- === SMALL BUILDINGS (third row, further back) ===

-- for i=1,25 do
--     local x_pos = -900 + i * 80
--     local z_pos = 200  -- Further back from main skyline
--     local height = 20 + math.random(50)
--     local width = 20 + math.random(20)
    
--     local building = gr.nh_box('s'..i, {x_pos, height/2, z_pos}, width)
--     scene:add_child(building)
--     building:set_material(concrete)
-- end

-- Lighting
sunlight = gr.light({1000.0, 800.0, 1000.0}, {0.9, 0.9, 0.8}, {1, 0, 0})
evening_light = gr.light({-800.0, 300.0, 800.0}, {0.6, 0.3, 0.1}, {1, 0, 0})
city_glow = gr.light({0.0, 100.0, 500.0}, {0.3, 0.3, 0.5}, {1, 0, 0})

-- -- Render the scene from a side view
-- gr.render(scene, 'nyc_skyline_side_view.png', 800, 400,
--     {0, 300, 800}, {0, 0, -1}, {0, 1, 0}, 50,
--     {0.1, 0.1, 0.3}, {sunlight, evening_light, city_glow})
gr.render(scene,
	  'scene0.png', 256, 256,
	  {0, 2, 30}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {gr.light({200, 202, 430}, {0.8, 0.8, 0.8}, {1, 0, 0})})