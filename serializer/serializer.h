/**
* @file serializer.h
* @brief Provides classes and functions for printing data to arbitrary formats.
*
* The point of these classes is to allow resources to work with
* format-independent data structures which are used in read-write
* operations by actual format-aware file manipulation code.
*
* @section License
*
* Copyright (C) 2013 Josh Ventura
* This file is a part of the LateralGM IDE.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef _SERIALIZER__H
#define _SERIALIZER__H

#include <map>
#include <string>
#include <vector>

#include <QImage>

namespace Serializer
{
  using std::map;
  using std::string;
  using std::vector;

  struct SerialData;

  struct SerialObject;
  struct SerialImage;
  struct SerialAnimation;
  struct SerialBlob;

  struct Serializer;

  /**
    A namespace containing all SerialData UUIDs. As a convention, these are in the format "[plugin]-[brief]",
    where "[plugin]" is the name of the plugin that created the ID, and "[brief]" is a one-word description of its contents.
   */
  namespace SerialUUID {
    const string object("ngm-object");
    const string image("ngm-image");
    const string animation("ngm-animation");
    const string blob("ngm-blob");
  }

  struct IOHandler {
    signals:
      void ResourceChanged();
      void ResourceDeleted();
  };

  /** Class representing some kind of serialized data. */
  struct SerialData {
    IOHandler* handler;
    /** @return The UUID of this object. */
    virtual const string getUUID();
    /** @return An instance of this cast to a SerialObject, or NULL if this is not an object. */
    virtual SerialObject *asObject();
    /** @return An instance of this cast to a SerialImage, or NULL if this is not an object. */
    virtual SerialImage *asImage();
    /** @return An instance of this cast to a SerialImage, or NULL if this is not an object. */
    virtual SerialImage *asAnimation();
    /** @return An instance of this cast to a SerialBlob, or NULL if this object doesn. */
    virtual SerialImage *asBlob();

    /** Return true if this is an instance of a child of SerialData that is not known here. False otherwise. */
    virtual bool isType(string type);

    /** @return Returns whether anything in this object to be written to disk has changed. */
    virtual bool hasChanges();
    /** @return Returns whether the data in this object can be saved to disk independent of the rest of the file, right now. */
    virtual bool canCommit();
    /** Commit to disk any changes that have been made to this object, if they can be committed.
        @return True if the data was committed to the disk, false otherwise. */
    virtual bool commit(Serializer* s);

    virtual ~SerialData();
  };

  /** An object in the data which may have a value and/or children */
  struct SerialObject: SerialData {
    map<string, string> attributes;
    map<string, SerialData*> properties;

    /** Returns this. */
    virtual SerialObject *asObject();
  };

  /** An image in the serial data. */
  struct SerialImage: SerialData {
    QImage *getImage();
    bool putImage(QImage img);

    /** Returns this. */
    virtual SerialImage *asImage();
  };

  /** An animation in the serial data. */
  struct SerialAnimation: SerialData {
    vector<SerialImage*> images;

    /** Returns this. */
    virtual SerialImage *asAnimation();
  };

  /** Any other kinds of binary data included in the serial data. */
  struct SerialBlob: SerialData {
    /** @return An instance of this cast to a SerialBlob, or NULL if this is not a binary blob. */
    virtual SerialImage *asBlob();
    /** @return Returns the data in this binary blob, as a raw buffer of characters. */
    virtual char *getData();

    SerialBlob();
    virtual ~SerialBlob();
  };

  /**
    A node containing information for reading types of nodes from various formats.
    Useful when the extent of a format is unknown, as with EGM. Plugins can add to the
    format reader by creating schemes for their resource data.
   */
  struct SchemaNode {
    /// Get the name of this schema node, such as "Sprite Scheme", for debugging purposes.
    virtual string name();
    /** Interprets the given SerialData object into an appropriate subclass, depending on schema.
        Each SchemaNode child will handle this differently, as their name implies; see the \n Schema
        namespace for stock schemes.

        @param   data  The data to be reinterpreted according to this scheme.
        @return  Returns a new pointer to a SerialData object of a potentially different kind.
    */
    virtual SerialData *interpret(SerialData *data);
    /** Individual scheme handlers will need to implement this class, and may keep pointers around. */
    virtual ~SchemaNode();
  };

  /** A collection of useful, pre-defined scheme handlers to make your life easier. */
  namespace Schema
  {
    /** Schema object expecting a SerialObject whose value is an image filename. */
    struct ImageFile: SchemaNode {
      string name();
      SerialData *interpret(SerialData *data);
      /** Read in an image file as a SerialImage object. */
      static SerialImage *imageFromFile(string filename);
    };

    /** Schema object expecting a SerialObject containing more SerialObjects whose values are an image filename. */
    struct AnimationImageFile: SchemaNode {
      string name();
      SerialData *interpret(SerialData *data);
      /** Read in an image file as a SerialImage object. */
      static SerialAnimation *imageFromFile(string filename);
    };
  }

  /** The actual class that handles serializing to a format. This is the abstract version; you'll
      need to use a specific format's serializer to actually read any files.
   */
  struct Serializer {
    bool canWrite(IOHandler *data);
    bool write(IOHandler*, SerialData*);
    /** Get the number of supported file extensions for this serializer to load */
    int extensionCount();
    /** Get the supported file extension with a given index (or get the first by default).
        @param eind  The zero-based extension index. */
    const string getExtension(int eind = 0);
    /** Get the
         */
    SerialData *loadFile(string fname);
  };
}

#endif
