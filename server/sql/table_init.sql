create table if not exists ender_label_user
(
    id             serial primary key,
    username       text not null,
    email          text not null,
    password       text not null,
    session        text,
    permission_ids int4[]
);

create table if not exists ender_label_permission
(
    id        serial primary key,
    key       varchar[32],
    parent_id int4,
    "desc"    text
);

ALTER TABLE ender_label_permission
    ADD CONSTRAINT fk_permission_parent
        FOREIGN KEY (parent_id)
            REFERENCES ender_label_permission (id);

create table if not exists ender_label_img_dataset
(
    id        serial primary key,
    owner_id  int4,
    name      text,
    "desc"    text,
    img_files text[],
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


