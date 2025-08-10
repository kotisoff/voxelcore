local body = entity.rigidbody

local jump_force = SAVED_DATA.jump_force or ARGS.jump_force or 0.0
local air_damping = SAVED_DATA.air_damping or ARGS.air_damping or 1.0
local ground_damping = SAVED_DATA.ground_damping or ARGS.ground_damping or 1.0
local movement_speed = SAVED_DATA.movement_speed or ARGS.movement_speed or 4.0
local run_speed_mul = SAVED_DATA.run_speed_mul or ARGS.run_speed_mul or 1.5
local crouch_speed_mul = SAVED_DATA.crouch_speed_mul or ARGS.crouch_speed_mul or 0.35
local flight_speed_mul = SAVED_DATA.flight_speed_mul or ARGS.flight_speed_mul or 4.0
local cheat_speed_mul = SAVED_DATA.cheat_speed_mul or ARGS.cheat_speed_mul or 5.0

function jump(multiplier)
    if body:is_grounded() then
        local vel = body:get_vel()
        body:set_vel(
            vec3.add(vel, {0, jump_force * (multiplier or 1.0), 0}, vel))
    end
end

function elevate(speed, delta, vel)
    vel = vel or body:get_vel()
    body:set_vel(
        vec3.add(vel, {0, speed * delta, 0}, vel))
end

function lower(speed, delta, vel)
    vel = vel or body:get_vel()
    body:set_vel(
        vec3.add(vel, {0, -speed * delta, 0}, vel))
end

function move_horizontal(speed, dir, vel)
    vel = vel or body:get_vel()
    if vec3.length(dir) > 0.0 then
        vec3.normalize(dir, dir)

        vel[1] = dir[1] * speed
        vel[3] = dir[3] * speed
    end
    body:set_vel(vel)
end

function on_update(tps)
    local delta = (1.0 / tps)
    local pid = entity:get_player()
    if pid then
        -- todo: replace with entity direction
        local cam = cameras.get("core:first-person")
        local front = cam:get_front()
        local right = cam:get_right()
        front[2] = 0.0
        vec3.normalize(front, front)

        local grounded = body:is_grounded()

        local isjump = input.is_active('movement.jump')
        local issprint = input.is_active('movement.sprint')
        local iscrouch = input.is_active('movement.crouch')
        local isforward = input.is_active('movement.forward')
        local ischeat = input.is_active('movement.cheat')
        local isback = input.is_active('movement.back')
        local isleft = input.is_active('movement.left')
        local isright = input.is_active('movement.right')
        local flight = player.is_flight(pid)
        local noclip = player.is_noclip(pid)

        local vel = body:get_vel()

        local speed = movement_speed

        if flight then
            speed = speed * flight_speed_mul
        elseif issprint then
            speed = speed * run_speed_mul
        elseif iscrouch and grounded then
            speed = speed * crouch_speed_mul
        end
        body:set_crouching(iscrouch)

        if ischeat then
            speed = speed * cheat_speed_mul
        end

        local dir = {0, 0, 0}
        if isforward then
            vec3.add(dir, front, dir)
        end
        if isback then
            vec3.sub(dir, front, dir)
        end
        if isright then
            vec3.add(dir, right, dir)
        end
        if isleft then
            vec3.sub(dir, right, dir)
        end

        if vec3.length(dir) > 0.0 then
            move_horizontal(speed, dir, vel)
        end

        if flight then
            if isjump then
                elevate(speed * 8.0, delta)
            elseif iscrouch then
                lower(speed * 8.0, delta)
            end
        elseif isjump then
            jump()
        end
        body:set_vdamping(flight)
        body:set_gravity_scale(flight and 0.0 or 1.0)
        body:set_linear_damping((flight or not grounded) and air_damping or ground_damping)
        body:set_body_type(noclip and "kinematic" or "dynamic")
    end
end
