alter table ender_label_user
    add constraint ender_label_user_pk
        unique (username, email);