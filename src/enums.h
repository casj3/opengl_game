#pragma once

enum State
{
    initializeAvatar,
    initializeScene1,
    scene1,
    editor
};

enum ProgramTypes
{
    default_program,
    skinning,
    normalMapSkinning,

    num_programs
};

enum SkeletonTypes
{
    avatar,

    num_skeletons
};

enum Surfaces
{
    test,

    num_surfaces
};
