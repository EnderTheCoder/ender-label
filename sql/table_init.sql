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

INSERT INTO ender_label_permission(key, parent_id, "desc")
VALUES ('DATASET_LIST', 1, 'List public datasets');

INSERT INTO ender_label_user(USERNAME, EMAIL, PASSWORD, TOKEN, PERMISSION_IDS)
VALUES ('root', 'root@test.com', '123456', null, ARRAY [1]);

create table if not exists ender_label_img_dataset
(
    id        serial primary key,
    owner_id  int4,
    name      text,
    "desc"    text,
    img_ids   int8[],
    class_ids int4[]
);

create table if not exists ender_label_img_dataset_annotation
(
    id           bigserial primary key,
    owner_id     int4,
    img_id       int8,
    task_type    text,
    raw_json     text,
    anno_cls_ids int4[]
);


create table if not exists ender_label_img
(
    id            bigserial primary key,
    relative_path text,
    size          int8,
    width         int4,
    height        int4,
    md5_hash_32   text
);

create table if not exists ender_label_annotation_class
(
    id     serial primary key,
    key    text,
    "desc" text
);

create table if not exists ender_label_annotation_task
(
    id             serial primary key,
    name           text,
    user_ids       int4[],
    dataset_id     int4,
    raw_json       text,
    state          bool,
    progress       float4,
    anno_task_type text
);

create table if not exists ender_label_annotation_log
(
    id       bigserial primary key,
    name     text,
    time     int8,
    task_id  int4,
    anno_id  int8,
    img_id   int8,
    user_id  int4,
    log_type text
);

create table if not exists ender_label_export_log
(
    id           serial primary key,
    dataset_id   int4,
    archive_path text,
    user_id      int4,
    begin_time   int8,
    end_time     int8,
    state        text,
    info         text
);