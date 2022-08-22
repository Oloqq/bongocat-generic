#include "header.hpp"

namespace osu {
Json::Value left_key_value, right_key_value, smoke_key_value, wave_key_value;
int offset_x, offset_y;
int paw_r, paw_g, paw_b, paw_a;
int paw_edge_r, paw_edge_g, paw_edge_b, paw_edge_a;
double scale;
bool is_left_handed, is_enable_toggle_smoke;
sf::Sprite desk, cat, up, left, right, device, smoke, wave, arm;
bool debug;
sf::CircleShape anchorMark, handMark;
sf::RectangleShape catsMouseMark;

int key_state = 0;

bool left_key_state = false;
bool right_key_state = false;
bool wave_key_state = false;
bool previous_smoke_key_state = false;
bool current_smoke_key_state = false;
bool is_toggle_smoke = false;
bool stretchy_arm = false;

double timer_left_key = -1;
double timer_right_key = -1;
double timer_wave_key = -1;
const double PI = 3.141592653589793238462643383279502884;
const double toDeg = 180.0 / PI;
double catsMouseAngle;

void toggle_debug() {
    debug = !debug;
}

bool init() {
    // getting configs
    Json::Value osu = data::cfg["osu"];

    stretchy_arm = osu["stretchyArm"].asBool();
    debug = osu["debug"].asBool();
    if (stretchy_arm) {
        anchorMark.setFillColor(sf::Color::Blue);
        anchorMark.setRadius(5);

        handMark.setFillColor(sf::Color::Red);
        handMark.setRadius(5);

        catsMouseMark.setFillColor(sf::Color::Transparent);
        catsMouseMark.setOutlineColor(sf::Color::Yellow);
        catsMouseMark.setOutlineThickness(2);

        double x, y;
        x = osu["armAnchorOffset"][0].asDouble();
        y = osu["armAnchorOffset"][1].asDouble();
        arm.setOrigin(x, y);

        x = osu["anchor"][0].asDouble();
        y = osu["anchor"][1].asDouble();
        anchorMark.setPosition(x, y);
        arm.setPosition(x, y);

        x = osu["catsMouse"][0].asDouble();
        y = osu["catsMouse"][1].asDouble();
        catsMouseMark.setPosition(x, y);
        x = osu["catsMouse"][2].asDouble();
        y = osu["catsMouse"][3].asDouble();
        catsMouseMark.setSize(sf::Vector2f(x, y));
        catsMouseAngle = osu["catsMouse"][4].asDouble();
        catsMouseMark.setRotation(catsMouseAngle);
    }
    is_enable_toggle_smoke = osu["toggleSmoke"].asBool();

    paw_r = osu["paw"][0].asInt();
    paw_g = osu["paw"][1].asInt();
    paw_b = osu["paw"][2].asInt();
    paw_a = osu["paw"].size() == 3 ? 255 : osu["paw"][3].asInt();

    paw_edge_r = osu["pawEdge"][0].asInt();
    paw_edge_g = osu["pawEdge"][1].asInt();
    paw_edge_b = osu["pawEdge"][2].asInt();
    paw_edge_a = osu["pawEdge"].size() == 3 ? 255 : osu["pawEdge"][3].asInt();

    bool chk[256];
    std::fill(chk, chk + 256, false);
    left_key_value = osu["key1"];
    for (Json::Value &v : left_key_value) {
        chk[v.asInt()] = true;
    }
    right_key_value = osu["key2"];
    for (Json::Value &v : right_key_value) {
        if (chk[v.asInt()]) {
            data::error_msg("Overlapping osu! keybinds", "Error reading configs");
            return false;
        }
    }
    wave_key_value = osu["wave"];
    for (Json::Value &v : wave_key_value) {
        if (chk[v.asInt()]) {
            data::error_msg("Overlapping osu! keybinds", "Error reading configs");
            return false;
        }
    }
    smoke_key_value = osu["smoke"];

    is_left_handed = data::cfg["decoration"]["leftHanded"].asBool();

    offset_x = (data::cfg["decoration"]["offsetX"])[0].asInt();
    offset_y = (data::cfg["decoration"]["offsetY"])[0].asInt();
    scale = (data::cfg["decoration"]["scalar"])[0].asDouble();

    // importing sprites
    up.setTexture(data::load_texture("img/osu/up.png"));
    left.setTexture(data::load_texture("img/osu/left.png"));
    right.setTexture(data::load_texture("img/osu/right.png"));
    wave.setTexture(data::load_texture("img/osu/wave.png"));
    cat.setTexture(data::load_texture("img/osu/avatar.png"));
    desk.setTexture(data::load_texture("img/osu/desk.png"));
    device.setTexture(data::load_texture("img/osu/mouse.png"), true);
    smoke.setTexture(data::load_texture("img/osu/smoke.png"));
    arm.setTexture(data::load_texture("img/osu/arm.png"));
    device.setScale(scale, scale);

    if (stretchy_arm) {
        auto b = device.getLocalBounds();
        device.setOrigin(b.width / 2, b.height / 2);
    }
    else {
        device.setOrigin(0, 0);
    }

    return true;
}

void draw_arm_fill(std::vector<double>& pss, std::vector<double>& pss2) {
    sf::VertexArray fill(sf::TriangleStrip, 26);
    for (int i = 0; i < 26; i += 2) {
        fill[i].position = sf::Vector2f(pss2[i], pss2[i + 1]);
        fill[i + 1].position = sf::Vector2f(pss2[52 - i - 2], pss2[52 - i - 1]);
        fill[i].color = sf::Color(paw_r, paw_g, paw_b, paw_a);
        fill[i + 1].color = sf::Color(paw_r, paw_g, paw_b, paw_a);
    }
    window.draw(fill);
}

void draw_arm_outline(std::vector<double>& pss, std::vector<double>& pss2) {
    // drawing first arm arc (solid black, rough edges)
    int shad = paw_edge_a / 3;
    sf::VertexArray edge(sf::TriangleStrip, 52);
    double width = 7;
    sf::CircleShape circ(width / 2);
    circ.setFillColor(sf::Color(paw_edge_r, paw_edge_g, paw_edge_b, shad));
    circ.setPosition(pss2[0] - width / 2, pss2[1] - width / 2);
    window.draw(circ);
    for (int i = 0; i < 50; i += 2) {
        double vec0 = pss2[i] - pss2[i + 2];
        double vec1 = pss2[i + 1] - pss2[i + 3];
        double dist = hypot(vec0, vec1);
        edge[i].position = sf::Vector2f(pss2[i] + vec1 / dist * width / 2, pss2[i + 1] - vec0 / dist * width / 2);
        edge[i + 1].position = sf::Vector2f(pss2[i] - vec1 / dist * width / 2, pss2[i + 1] + vec0 / dist * width / 2);
        edge[i].color = sf::Color(paw_edge_r, paw_edge_g, paw_edge_b, shad);
        edge[i + 1].color = sf::Color(paw_edge_r, paw_edge_g, paw_edge_b, shad);
        width -= 0.08;
    }
    double vec0 = pss2[50] - pss2[48];
    double vec1 = pss2[51] - pss2[49];
    double dist = hypot(vec0, vec1);
    edge[51].position = sf::Vector2f(pss2[50] + vec1 / dist * width / 2, pss2[51] - vec0 / dist * width / 2);
    edge[50].position = sf::Vector2f(pss2[50] - vec1 / dist * width / 2, pss2[51] + vec0 / dist * width / 2);
    edge[50].color = sf::Color(paw_edge_r, paw_edge_g, paw_edge_b, shad);
    edge[51].color = sf::Color(paw_edge_r, paw_edge_g, paw_edge_b, shad);
    window.draw(edge);
    circ.setRadius(width / 2);
    circ.setPosition(pss2[50] - width / 2, pss2[51] - width / 2);
    window.draw(circ);

    // drawing second arm arc (shadow)
    sf::VertexArray edge2(sf::TriangleStrip, 52);
    width = 6;
    sf::CircleShape circ2(width / 2);
    circ2.setFillColor(sf::Color(paw_edge_r, paw_edge_g, paw_edge_b, paw_edge_a));
    circ2.setPosition(pss2[0] - width / 2, pss2[1] - width / 2);
    window.draw(circ2);
    for (int i = 0; i < 50; i += 2) {
        vec0 = pss2[i] - pss2[i + 2];
        vec1 = pss2[i + 1] - pss2[i + 3];
        double dist = hypot(vec0, vec1);
        edge2[i].position = sf::Vector2f(pss2[i] + vec1 / dist * width / 2, pss2[i + 1] - vec0 / dist * width / 2);
        edge2[i + 1].position = sf::Vector2f(pss2[i] - vec1 / dist * width / 2, pss2[i + 1] + vec0 / dist * width / 2);
        edge2[i].color = sf::Color(paw_edge_r, paw_edge_g, paw_edge_b, paw_edge_a);
        edge2[i + 1].color = sf::Color(paw_edge_r, paw_edge_g, paw_edge_b, paw_edge_a);
        width -= 0.08;
    }
    vec0 = pss2[50] - pss2[48];
    vec1 = pss2[51] - pss2[49];
    dist = hypot(vec0, vec1);
    edge2[51].position = sf::Vector2f(pss2[50] + vec1 / dist * width / 2, pss2[51] - vec0 / dist * width / 2);
    edge2[50].position = sf::Vector2f(pss2[50] - vec1 / dist * width / 2, pss2[51] + vec0 / dist * width / 2);
    edge2[50].color = sf::Color(paw_edge_r, paw_edge_g, paw_edge_b, paw_edge_a);
    edge2[51].color = sf::Color(paw_edge_r, paw_edge_g, paw_edge_b, paw_edge_a);
    window.draw(edge2);
    circ2.setRadius(width / 2);
    circ2.setPosition(pss2[50] - width / 2, pss2[51] - width / 2);
    window.draw(circ2);
}

void draw_stretchy_arm() {
    auto [x, y] = input::where_mouse();

    sf::Transform t;
    t.translate(catsMouseMark.getPosition());
    t.rotate(catsMouseAngle, 0, 0);
    t.scale(catsMouseMark.getSize());
    sf::Vector2f handPos = t.transformPoint(sf::Vector2f(x, y));

    handMark.setPosition(handPos);
    sf::Vector2f displacement = handPos - anchorMark.getPosition();

    double dist = hypot(displacement.x, displacement.y);
    double scale = dist / arm.getLocalBounds().height;
    arm.setScale(1, scale);
    double alpha = asin(-displacement.x / dist);
    double deg = alpha * toDeg;
    arm.setRotation(deg);

    device.setPosition(handPos);
    window.draw(device);

    window.draw(arm);
}

void draw() {
    window.draw(desk);

    draw_stretchy_arm();

    window.draw(cat);
    if (debug && stretchy_arm) {
        window.draw(handMark);
        window.draw(anchorMark);
        window.draw(catsMouseMark);
    }
    // drawing keypresses
    bool left_key = false;

    for (Json::Value &v : left_key_value) {
        if (input::is_pressed(v.asInt())) {
            left_key = true;
            break;
        }
    }

    if (left_key) {
        if (!left_key_state) {
            key_state = 1;
            left_key_state = true;
        }
    } else {
        left_key_state = false;
    }

    bool right_key = false;

    for (Json::Value &v : right_key_value) {
        if (input::is_pressed(v.asInt())) {
            right_key = true;
            break;
        }
    }

    if (right_key) {
        if (!right_key_state) {
            key_state = 2;
            right_key_state = true;
        }
    } else {
        right_key_state = false;
    }

    bool wave_key = false;

    for (Json::Value &v : wave_key_value) {
        if (input::is_pressed(v.asInt())) {
            wave_key = true;
            break;
        }
    }

    if (wave_key) {
        if (!wave_key_state) {
            key_state = 3;
            wave_key_state = true;
        }
    } else {
        wave_key_state = false;
    }

    if (!left_key_state && !right_key_state && !wave_key_state) {
        key_state = 0;
        window.draw(up);
    }

    if (key_state == 1) {
        if ((clock() - std::max(timer_right_key, timer_wave_key)) / CLOCKS_PER_SEC > BONGO_KEYPRESS_THRESHOLD) {
            if (!is_left_handed) {
                window.draw(left);
            } else {
                window.draw(right);
            }
            timer_left_key = clock();
        } else {
            window.draw(up);
        }
    } else if (key_state == 2) {
        if ((clock() - std::max(timer_left_key, timer_wave_key)) / CLOCKS_PER_SEC > BONGO_KEYPRESS_THRESHOLD) {
            if (!is_left_handed) {
                window.draw(right);
            } else {
                window.draw(left);
            }
            timer_right_key = clock();
        } else {
            window.draw(up);
        }
    } else if (key_state == 3) {
        if ((clock() - std::max(timer_left_key, timer_right_key)) / CLOCKS_PER_SEC > BONGO_KEYPRESS_THRESHOLD) {
            window.draw(wave);
            timer_wave_key = clock();
        } else {
            window.draw(up);
        }
    }

    // draw smoke
    bool is_smoke_key_pressed = false;

    for (Json::Value &v : smoke_key_value) {
        if (input::is_pressed(v.asInt())) {
            is_smoke_key_pressed = true;
            break;
        }
    }

    if (is_enable_toggle_smoke) {
        previous_smoke_key_state = current_smoke_key_state;
        current_smoke_key_state = is_smoke_key_pressed;

        bool is_smoke_key_down = current_smoke_key_state && (current_smoke_key_state != previous_smoke_key_state);

        if (is_smoke_key_down) {
            is_toggle_smoke = !is_toggle_smoke;
        }
    }
    else {
        is_toggle_smoke = is_smoke_key_pressed;
    }

    if (is_toggle_smoke) {
        window.draw(smoke);
    }
}
}; // namespace osu
