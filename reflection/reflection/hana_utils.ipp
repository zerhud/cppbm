/* Copyright (C) 2024 Hudyaev Alexey
 * SPDX-License-Identifier:    MIT
 */

template<typename type> constexpr auto decay(const type&) { return type_c<decay_t<type>>; }

