/**
 * @file kernel/chan.rs
 * @version 0.3.0
 *
 * @section License
 * Copyright (C) 2014-2016, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Simba project.
 */

pub type Chan = ::Struct_chan_t;

pub trait ChanHandleTrait {

    fn get_chan_p(&self) -> *mut ::std::os::raw::c_void;

    fn write(&self, buf: &[u8]) -> ::Res;

    fn read(&self, buf: &mut [u8]) -> ::Res;
}