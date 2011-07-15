//
//  CouchDocument.h
//  CouchCocoa
//
//  Created by Jens Alfke on 5/26/11.
//  Copyright 2011 Couchbase, Inc. All rights reserved.
//

#import "CouchResource.h"
@class CouchAttachment, CouchDatabase, CouchRevision;


/** A CouchDB document, aka "record" aka "row".
    Note: Never alloc/init a CouchDocument directly. Instead get it from the database by calling -documentWithID: or -untitledDocument. */
@interface CouchDocument : CouchResource
{
    @private
    id _modelObject;
    BOOL _isDeleted;
    NSString* _currentRevisionID;
    CouchRevision* _currentRevision;
}


@property (readonly) NSString* documentID;
@property (readonly) BOOL isDeleted;

/** Optional reference to an application-defined model object representing this document.
    This property is unused and uninterpreted by CouchCocoa; use it for whatever you want.
    Note that this is not a strong/retained reference. */
@property (assign) id modelObject;

#pragma mark REVISIONS:

/** The ID of the current revision (if known). */
@property (readonly, copy) NSString* currentRevisionID;

/** The current/latest revision. This object is cached. */
- (CouchRevision*) currentRevision;

/** The revision with the specified ID.
    This is merely a factory method that doesn't fetch anything from the server,
    or even verify that the ID is valid. */
- (CouchRevision*) revisionWithID: (NSString*)revisionID;

/** Returns an array of available revisions, in basically forward chronological order. */
- (NSArray*) getRevisionHistory;

#pragma mark PROPERTIES:

/** These are the app-defined properties of the document, without the CouchDB-defined special properties whose names begin with "_".
    This is shorthand for self.currentRevision.properties.
    (If you want the entire document object returned by the server, get the revision's -contents property.) */
@property (readonly, copy) NSDictionary* properties;

/** Shorthand for [self.properties objectForKey: key]. */
- (id) propertyForKey: (NSString*)key;

/** Updates the document with new properties.
    This is asynchronous. Watch response for conflicts! */
- (RESTOperation*) putProperties: (NSDictionary*)properties;

#pragma mark CONFLICTS:

/** Returns an array of revisions that are currently in conflict, in no particular order.
    If there is no conflict, returns an array of length 1 containing only the current revision.
    Returns nil if an error occurs. */
- (NSArray*) getConflictingRevisions;

/** Resolves a conflict by choosing one existing revision as the winner.
    (This is the same as calling -resolveConflictingRevisions:withProperties:, passing in
    winningRevision.properties.)
    @param conflicts  The array of conflicting revisions as returned by -getConflictingRevisions.
    @param winningRevision  The revision from 'conflicts' whose properties should be used. */
- (RESTOperation*) resolveConflictingRevisions: (NSArray*)conflicts 
                                  withRevision: (CouchRevision*)winningRevision;

/** Resolves a conflict by creating a new winning revision from the given properties.
    @param conflicts  The array of conflicting revisions as returned by -getConflictingRevisions.
    @param properties  The properties to store into the document to resolve the conflict. */
- (RESTOperation*) resolveConflictingRevisions: (NSArray*)conflicts
                                withProperties: (NSDictionary*)properties;

@end


/** This notification is posted by a CouchDocument in response to an external change (as reported by the _changes feed.)
    It is not sent in response to 'local' changes made by this CouchDatabase's object tree.
    It will not be sent unless change-tracking is enabled in its parent CouchDatabase. */
extern NSString* const kCouchDocumentChangeNotification;


@protocol CouchDocumentModel <NSObject>
- (void) couchDocumentChanged: (CouchDocument*)doc;
@end