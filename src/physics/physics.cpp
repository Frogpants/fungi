#include "physics.hpp"

static const vec3 GRAVITY = vec3(0.0f, -9.81f, 0.0f);

static bool AABBOverlap(const BoxBody& a, const BoxBody& b) {
    vec3 delta = abs(a.body.position - b.body.position);
    vec3 total = a.collider.halfSize + b.collider.halfSize;

    return (delta.x <= total.x &&
            delta.y <= total.y &&
            delta.z <= total.z);
}

static vec3 GetPenetration(const BoxBody& a, const BoxBody& b) {
    vec3 delta = abs(a.body.position - b.body.position);
    vec3 total = a.collider.halfSize + b.collider.halfSize;
    return total - delta;
}

void PhysicsStep(std::vector<BoxBody>& bodies) {

    // -------- Integration --------
    for (BoxBody& b : bodies) {
        if (b.body.isStatic) continue;

        // Apply gravity
        b.body.force += GRAVITY * b.body.mass;

        // Integrate velocity
        vec3 acceleration = b.body.force / b.body.mass;
        b.body.velocity += acceleration * deltaTime;
        b.body.position += b.body.velocity * deltaTime;

        // Clear forces
        b.body.force = vec3(0.0f);
    }

    // -------- Collision detection & resolution --------
    for (size_t i = 0; i < bodies.size(); i++) {
        for (size_t j = i + 1; j < bodies.size(); j++) {

            BoxBody& A = bodies[i];
            BoxBody& B = bodies[j];

            if (!AABBOverlap(A, B)) continue;

            vec3 penetration = GetPenetration(A, B);

            // Find smallest penetration axis
            if (penetration.x < penetration.y && penetration.x < penetration.z) {
                float dir = sign(A.body.position.x - B.body.position.x);
                vec3 correction = vec3(penetration.x * dir, 0, 0);

                if (!A.body.isStatic) A.body.position += correction * 0.5f;
                if (!B.body.isStatic) B.body.position -= correction * 0.5f;

                A.body.velocity.x = 0;
                B.body.velocity.x = 0;
            }
            else if (penetration.y < penetration.z) {
                float dir = sign(A.body.position.y - B.body.position.y);
                vec3 correction = vec3(0, penetration.y * dir, 0);

                if (!A.body.isStatic) A.body.position += correction * 0.5f;
                if (!B.body.isStatic) B.body.position -= correction * 0.5f;

                A.body.velocity.y = 0;
                B.body.velocity.y = 0;
            }
            else {
                float dir = sign(A.body.position.z - B.body.position.z);
                vec3 correction = vec3(0, 0, penetration.z * dir);

                if (!A.body.isStatic) A.body.position += correction * 0.5f;
                if (!B.body.isStatic) B.body.position -= correction * 0.5f;

                A.body.velocity.z = 0;
                B.body.velocity.z = 0;
            }
        }
    }
}
