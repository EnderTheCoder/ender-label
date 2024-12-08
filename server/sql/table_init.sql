create table if not exists ender_label_user
(
    id             serial primary key,
    username       text not null,
    email          text not null,
    password       text not null,
    token          text,
    permission_ids int4[]
);

create table if not exists ender_label_permission
(
    id        serial primary key,
    key       text,
    parent_id int4,
    "desc"    text
);

ALTER TABLE ender_label_permission
    ADD CONSTRAINT fk_permission_parent
        FOREIGN KEY (parent_id)
            REFERENCES ender_label_permission (id);

INSERT INTO ender_label_permission(key, parent_id, "desc")
VALUES ('ROOT', null, 'ROOT');
INSERT INTO ender_label_user(USERNAME, EMAIL, PASSWORD, TOKEN, PERMISSION_IDS)
VALUES ('root', 'root@test.com', '123456', null, ARRAY [1]);

create table if not exists ender_label_img_dataset
(
    id        serial primary key,
    owner_id  int4,
    name      text,
    "desc"    text,
    img_ids int8[],
    class_ids int4[]
);

create table if not exists ender_label_img_dataset_annotation
(
    id           serial primary key,
    owner_id     int4,
    img_name     text,
    type         text,
    raw_json     json,
    width        int8,
    height       int8,
    anno_cls_ids int4[]
);


create table if not exists ender_label_img
(
    id            bigserial primary key,
    relative_path text,
    size          int8,
    width         int4,
    height        int4,
    md5_hash_32   varchar[32]
)